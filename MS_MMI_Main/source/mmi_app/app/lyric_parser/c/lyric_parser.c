
#include "std_header.h"
#include "window_parse.h"
#include "mmk_timer.h"
#include "mmk_app.h"
#include "lyric_parser.h"
#include "zmt_main_file.h"

#define LYRIC_READ_BUFFER_SIZE 1024

#define LYRIC_LINE_SYM_RN "\r\n" // windows
#define LYRIC_LINE_SYM_N "\n" // linux
#define LYRIC_LINE_SYM_R "\r" // macos

// #define PRINT_SAVE_FILE
#ifdef PRINT_SAVE_FILE
    #define PRINT_SAVE_FILE_NAME "D:/lyric_print.txt"
#endif

#define LYRIC_READ_TEXT_MAX_COUNT 10

static lyric_content_node *lyric_list_node_create(long timems, char *content, int content_len)
{
    lyric_content_node *node = (lyric_content_node *)SCI_ALLOC_APPZ(sizeof(lyric_content_node));
    memset(node, 0, sizeof(lyric_content_node));
    node->timems = timems;
    if (content != NULL) {
        node->content_len = content_len;
        node->content = (char *) SCI_ALLOC_APPZ(content_len + 1);
        memset(node->content, 0, content_len + 1);
        // strncpy(node->content, content, content_len);
        memcpy(node->content, content, content_len);
    }
    node->next = NULL;
    return node;
}

static void lyric_list_node_free(lyric_content_node *node)
{
    if (node != NULL) {
        node->timems = 0;
        if (node->content != NULL) {
            SCI_FREE((void *)node->content);
            node->content = NULL;
        }
        node->next = NULL;
        SCI_FREE((void *)node);
        node = NULL;
    }
}

static bool lyric_list_insert(lyric_content_node *head, long timems, char *content, int content_len)
{
    lyric_content_node *node = lyric_list_node_create(timems, content, content_len);
    lyric_content_node *current = head; // head node, not first node
    if (node == NULL) {
        LP_ERROR("%s: create lyric list node failed \n", __FUNCTION__);
        return false;
    }
    
    while(current != NULL) {
        if (current->next == NULL) {
            current->next = node;
            break;
        } else {
            if (current->next->timems > timems) {
                node->next = current->next;
                current->next = node;
                break;
            } else {
                current = current->next;
            }
        }
    }
    return true;
}

static void lyric_list_destroy(lyric_content_node *head)
{
    lyric_content_node *current = head;
    while (current != NULL) {
        head = head->next;
        lyric_list_node_free(current);
        current = head;
    }
}

void lyric_print(lyric_t *lrc_p)
{
    lyric_content_node *current = NULL;
    MMIFILE_HANDLE file_handle = 0;
    wchar lrc_path[100] = {0};
    uint32 size_write = 0;
    LP_ERROR("%s: source file code:%d\n", __FUNCTION__, lrc_p->code);
    LP_ERROR("%s: source file line symbol:%d\n", __FUNCTION__, lrc_p->line_sym);
    if (lrc_p->author != NULL) {
        LP_ERROR("%s: lyric author:%s\n", __FUNCTION__, lrc_p->author);
    }
    if (lrc_p->by != NULL) {
        LP_ERROR("%s: lyric by:%s\n", __FUNCTION__, lrc_p->by);
    }
    if (lrc_p->album != NULL) {
        LP_ERROR("%s: lyric album:%s\n", __FUNCTION__, lrc_p->album);
    }
    if (lrc_p->artist != NULL) {
        LP_ERROR("%s: lyric artist:%s\n", __FUNCTION__, lrc_p->artist);
    }
    if (lrc_p->re != NULL) {
        LP_ERROR("%s: lyric re:%s\n", __FUNCTION__, lrc_p->re);
    }
    if (lrc_p->title != NULL) {
        LP_ERROR("%s: lyric title:%s\n", __FUNCTION__, lrc_p->title);
    }
    if (lrc_p->version != NULL) {
        LP_ERROR("%s: lyric version:%s\n", __FUNCTION__, lrc_p->version);
    }
    LP_ERROR("%s: lyric offset:%d\n", __FUNCTION__, lrc_p->offset);
    // print list
    current = lrc_p->lyric_list_head->next;
    #ifdef PRINT_SAVE_FILE
    GUI_GBToWstr(lrc_path, PRINT_SAVE_FILE_NAME, strlen(PRINT_SAVE_FILE_NAME));
    file_handle = MMIAPIFMM_CreateFile(lrc_path, SFS_MODE_READ | SFS_MODE_OPEN_EXISTING, PNULL, PNULL);   
    #endif
    while (current != NULL) {
        int milsec = (current->timems % 1000) / 10;
        int secs = current->timems / 1000;
        int sec = secs % 60;
        int min = secs / 60;
        LP_ERROR("%s: timems:%d format_time:[%02d:%02d.%02d] content:%s\n", __FUNCTION__, current->timems, min, sec, milsec, current->content);
        #ifdef PRINT_SAVE_FILE
        if (file_handle > 0) {
            int line_len = 10 + strlen(current->content) + 2 + 1; // [mm:ss.xx]content\r\n
            char *buf_tmp = SCI_ALLOC_APPZ(line_len);
            memset(buf_tmp, 0, line_len);
            sprintf(buf_tmp, "[%02d:%02d.%02d]%s\r\n", min, sec, milsec, current->content);
            LP_ERROR("%s: buf_tmp_len:%d line_len:%d\n", __FUNCTION__, strlen(buf_tmp), line_len);
            MMIFILE_WriteFile(file_handle, buf_tmp, strlen(buf_tmp), &size_write, PNULL);
            SCI_FREE(buf_tmp);
        }
        #endif
        current = current->next;
    }
    #ifdef PRINT_SAVE_FILE
    if (file_handle > 0) {
        MMIFILE_CloseFile(file_handle);
    }
    #endif
}

static bool is_value_space(const char *value, int value_len)
{
    int i = 0;
    for (i = 0; i < value_len; i++) {
        if (value[i] != ' ' && value[i] != '\t'
            && value[i] != '\r' && value[i] != '\n'
            && value[i] != '\v' && value[i] != '\f') {
            return false;
        }
    }
    return true;
}

static void lyric_parse_line(lyric_t *lrc_p, char *line, int line_len)
{
    int label_total_len = 0;
    int content_len = 0;
    char *last_label_end = strrchr(line, ']');
    char *line_buf_p = NULL;
    char *label_start = NULL;
    char *label_end = NULL;
    char *label_value = NULL;
    bool empty_content;
    LP_DEBUG("%s: line_len:%d line:%s\n", __FUNCTION__, line_len, line);
    
    if (last_label_end == NULL) {
        LP_ERROR("%s: wrong format 111!!!\n", __FUNCTION__);
        return;
    }
    label_total_len = last_label_end - line + 1;
    content_len = line_len - label_total_len;
    if (content_len < 0) {
        LP_ERROR("%s: wrong format 222!!!\n", __FUNCTION__);
        return;
    }

    empty_content = (content_len == 0) || is_value_space(&line[label_total_len], content_len);
    LP_DEBUG("%s: empty_content:%d\n", __FUNCTION__, empty_content);

    line_buf_p = line;
    while ((label_start = strchr(line_buf_p, '[')) != NULL && (label_end = strchr(line_buf_p, ']')) != NULL) {
        int label_start_pos = label_start - line_buf_p + 1;
        int label_end_pos = label_end - line_buf_p - 1;
        int label_value_len = label_end_pos - label_start_pos + 1;
        LP_DEBUG("%s: start_pos:%d end_pos:%d, content_len:%d\n", __FUNCTION__, label_start_pos, label_end_pos, label_value_len);
        if (label_value_len <= 0) { // at least "[]"
            break;
        }

        label_value = (char *)SCI_ALLOC_APPZ(label_value_len + 1);
        memset(label_value, 0, label_value_len + 1);
        memcpy(label_value, line_buf_p + label_start_pos, label_value_len);
        LP_DEBUG("%s: label_value_len:%d, strlen:%d, label_value:%s\n", __FUNCTION__, label_value_len, strlen(label_value), label_value);
        if (strncmp(label_value, "au:", 3) == 0) {
            if (label_value_len > 3) {
                int real_content_len = label_value_len - 3;
                lrc_p->author = (char *)SCI_ALLOC_APPZ(real_content_len + 1);
                if (lrc_p->author  != NULL) {
                    memset(lrc_p->author, 0, real_content_len + 1);
                    memcpy(lrc_p->author, &label_value[3], real_content_len);
                    LP_DEBUG("%s: content_len:%d author:%s\n", __FUNCTION__, real_content_len, lrc_p->author);
                } else {
                    LP_ERROR("%s: author alloc memory failed\n", __FUNCTION__);
                }
            }
        } else if (strncmp(label_value, "by:", 3) == 0) {
            if (label_value_len > 3) {
                int real_content_len = label_value_len - 3;
                lrc_p->by = (char *)SCI_ALLOC_APPZ(real_content_len + 1);
                if (lrc_p->by  != NULL) {
                    memset(lrc_p->by, 0, real_content_len + 1);
                    memcpy(lrc_p->by, &label_value[3], real_content_len);
                    LP_DEBUG("%s: content_len:%d by:%s\n", __FUNCTION__, real_content_len, lrc_p->by);
                } else {
                    LP_ERROR("%s: by alloc memory failed\n", __FUNCTION__);
                }
            }
        } else if (strncmp(label_value, "al:", 3) == 0) {
            if (label_value_len > 3) {
                int real_content_len = label_value_len - 3;
                lrc_p->album = (char *)SCI_ALLOC_APPZ(real_content_len + 1);
                if (lrc_p->album != NULL) {
                    memset(lrc_p->album, 0, real_content_len + 1);
                    memcpy(lrc_p->album, &label_value[3], real_content_len);
                    LP_DEBUG("%s: content_len:%d album:%s\n", __FUNCTION__, real_content_len, lrc_p->album);
                } else {
                    LP_ERROR("%s: album alloc memory failed\n", __FUNCTION__);
                }
            }
        } else if (strncmp(label_value, "ar:", 3) == 0) {
            if (label_value_len > 3) {
                int real_content_len = label_value_len - 3;
                lrc_p->artist = (char *)SCI_ALLOC_APPZ(real_content_len + 1);
                if (lrc_p->artist != NULL) {
                    memset(lrc_p->artist, 0, real_content_len + 1);
                    memcpy(lrc_p->artist, &label_value[3], real_content_len);
                    LP_DEBUG("%s: content_len:%d artist:%s\n", __FUNCTION__, real_content_len, lrc_p->artist);
                } else {
                    LP_ERROR("%s: artist alloc memory failed\n", __FUNCTION__);
                }
            }
        } else if (strncmp(label_value, "re:", 3) == 0) {
            if (label_value_len > 3) {
                int real_content_len = label_value_len - 3;
                lrc_p->re = (char *)SCI_ALLOC_APPZ(real_content_len + 1);
                if (lrc_p->re != NULL) {
                    memset(lrc_p->re, 0, real_content_len + 1);
                    memcpy(lrc_p->re, &label_value[3], real_content_len);
                    LP_DEBUG("%s: content_len:%d re:%s\n", __FUNCTION__, real_content_len, lrc_p->re);
                } else {
                    LP_ERROR("%s: re alloc memory failed\n", __FUNCTION__);
                }
            }
        } else if (strncmp(label_value, "tl:", 3) == 0) {
            if (label_value_len > 3) {
                int real_content_len = label_value_len - 3;
                lrc_p->title = (char *)SCI_ALLOC_APPZ(real_content_len + 1);
                if (lrc_p->title != NULL) {
                    memset(lrc_p->title, 0, real_content_len + 1);
                    memcpy(lrc_p->title, &label_value[3], real_content_len);
                    LP_DEBUG("%s: content_len:%d title:%s\n", __FUNCTION__, real_content_len, lrc_p->title);
                } else {
                    LP_ERROR("%s: title alloc memory failed\n", __FUNCTION__);
                }
            }
        } else if (strncmp(label_value, "ve:", 3) == 0) {
            if (label_value_len > 3) {
                int real_content_len = label_value_len - 3;
                lrc_p->version = (char *) SCI_ALLOC_APPZ(real_content_len + 1);
                if (lrc_p->version != NULL) {
                    memset(lrc_p->version, 0, real_content_len + 1);
                    memcpy(lrc_p->version, &label_value[3], real_content_len);
                    LP_DEBUG("%s: content_len:%d version:%s\n", __FUNCTION__, real_content_len, lrc_p->version);
                } else {
                    LP_ERROR("%s: version alloc memory failed\n", __FUNCTION__);
                }
            }
        } else if (strncmp(label_value, "offset:", 7) == 0) {
            int real_content_len = 0;
            char *offset;
            if (label_value_len > 8) {
                int sign = 1;
                if (label_value[7] == '-') {
                    sign = -1;
                }
                real_content_len = label_value_len - 8;
                offset = (char *) SCI_ALLOC_APPZ(real_content_len + 1);
                if (offset != NULL) {
                    memset(offset, 0, real_content_len + 1);
                    memcpy(offset, &label_value[8], real_content_len);
                    lrc_p->offset = atoi(offset) * sign;
                    LP_DEBUG("%s: offset:%d\n", __FUNCTION__, lrc_p->offset);
                    SCI_FREE((void *) offset);
                } else {
                    LP_ERROR("%s: offset alloc memory failed\n", __FUNCTION__);
                }
            }
        } else {
            int min = 0;
            int sec = 0;
            int milsec = 0;
            LP_DEBUG("%s: filter invalid line\n", __FUNCTION__);
            if (sscanf(label_value, "%d:%d.%d", &min, &sec, &milsec) > 0) {
                LP_DEBUG("%s: min:%d sec:%d milsec:%d empty_content:%d\n", __FUNCTION__, min, sec, milsec, empty_content);
                if (!empty_content) {
                    long timems = milsec > 100 ? (min * 60 + sec) * 1000 + milsec : (min * 60 + sec) * 1000 + milsec * 10;                  
                    bool ret = lyric_list_insert(lrc_p->lyric_list_head, timems, &line[label_total_len], content_len);
                    LP_DEBUG("%s: timems = %d\n", __FUNCTION__, timems);
                    if (ret) {
                        lrc_p->lyric_num++;
                    }
                }
            }
        }

        SCI_FREE(label_value);

        line_buf_p += label_value_len + 2;
        LP_DEBUG("%s: line_buf_p:%s\n", __FUNCTION__, line_buf_p);
    }
}

static int lyric_parser(lyric_t *lrc_p, const char *lrc_name)
{
    uint8 i = 0;
    INT32 ret = 0;
    MMIFILE_ERROR_E result = SFS_NO_ERROR;
    int left = 0;
    char *line_sign;
    int line_sign_len;
    char buf[LYRIC_READ_BUFFER_SIZE] = {0};
    char line_buf[LYRIC_READ_BUFFER_SIZE * 2] = {0};
    wchar lrc_path[100] = {0};
    uint32 read_size = 0;
    MMIFILE_HANDLE handle = 0;
    GUI_GBToWstr(lrc_path, lrc_name, strlen(lrc_name));
    handle = MMIAPIFMM_CreateFile(lrc_path, SFS_MODE_READ | SFS_MODE_OPEN_EXISTING, PNULL, PNULL);   
    if (handle <= 0) {
        LP_ERROR("%s: open lrc file failed handle:%d!!!\n", __FUNCTION__, handle);
        return -1;
    }

    lrc_p->code = LP_UNKNOW;
    lrc_p->line_sym = LP_SYM_UNKNOW;
    for(i = 0;i < LYRIC_READ_TEXT_MAX_COUNT;i++)
    {
        result = MMIFILE_ReadFile(handle, buf, LYRIC_READ_BUFFER_SIZE, &read_size, PNULL);
        LP_ERROR("%s: result:%d\n", __FUNCTION__, result);
        if(result != SFS_NO_ERROR)
        {
            LP_ERROR("%s: file read end\n", __FUNCTION__);
            break;
        }
        else
        {
            char *buf_start_p = &buf[0];
            char *line_end_p = NULL;
            uint32 handle_line_num = 0;

            LP_DEBUG("%s: read_size:%d\n", __FUNCTION__, read_size);
            if (lrc_p->code == LP_UNKNOW) {
                if ((buf[0] == 0xFE && buf[1] == 0xFF) || (buf[0] == 0xFF && buf[1] == 0xFE)) {
                    lrc_p->code = (buf[0] == 0xFE) ? LP_UNICODE_B : LP_UNICODE_L;
                    buf_start_p = &buf[2];
                } else if (buf[0] == 0xEF && buf[1] == 0xBB && buf[2] == 0xBF) {
                     lrc_p->code = LP_UTF8;
                     buf_start_p = &buf[3];
                } else {
                    lrc_p->code = LP_ASCII;
                }
            }

            if (lrc_p->line_sym == LP_SYM_UNKNOW) {
                char *r = strstr(buf_start_p, LYRIC_LINE_SYM_R);
                char *n = strstr(buf_start_p, LYRIC_LINE_SYM_N);
                if (r != NULL && n == NULL) {
                    lrc_p->line_sym = LP_SYM_MAC;
                    line_sign = LYRIC_LINE_SYM_R;
                    line_sign_len = 1;
                } else if (n != NULL && r == NULL) {
                    lrc_p->line_sym = LP_SYM_UNIX;
                    line_sign = LYRIC_LINE_SYM_N;
                    line_sign_len = 1;
                } else {
                    lrc_p->line_sym = LP_SYM_WIN;
                    line_sign = LYRIC_LINE_SYM_RN;
                    line_sign_len = 2;
                }
            }

            while((line_end_p = strstr(buf_start_p, line_sign)) != NULL) {
                uint32 line_len = line_end_p - buf_start_p;

                memcpy(&line_buf[left], buf_start_p, line_len);
                handle_line_num += line_len + line_sign_len;
                LP_DEBUG("%s: line_len:%d handle_line_num:%d line_buf:%s\n", __FUNCTION__, line_len, handle_line_num, line_buf);

                // parse line
                lyric_parse_line(lrc_p, &line_buf[0], line_len + left);

                // reset line buffer
                left = 0;
                memset(line_buf, 0, LYRIC_READ_BUFFER_SIZE * 2);

                // handle more than read
                if (handle_line_num >= read_size) {
                    break;
                }
                buf_start_p = line_end_p + line_sign_len;
            }

            left = read_size - handle_line_num;
            LP_DEBUG("%s: read_size:%d, handle_line_num:%d, left:%d\n", __FUNCTION__, read_size, handle_line_num, left);
            if (left > 0) {
                memcpy(line_buf, &buf[handle_line_num], left);
            } else {
                left = 0;
            }
        }
    }
    ret = MMIFILE_CloseFile(handle);
    LP_DEBUG("%s: close lrc file ret:%d!!!\n", __FUNCTION__, ret);

    return 0;
}

lyric_t *lyric_init(char *lrc_name)
{
    int ret = 0;
    lyric_t *lrc_p = NULL;
    LP_DEBUG("%s: lrc_name:%s\n", __FUNCTION__, lrc_name);
    if (lrc_name == NULL || strlen(lrc_name) == 0) {
        LP_ERROR("%s: please check lrc name!!!\n", __FUNCTION__);
        return NULL;
    }

    lrc_p = (lyric_t *) SCI_ALLOC_APPZ(sizeof(lyric_t));
    if (lrc_p == NULL) {
        LP_ERROR("%s: alloc memory for lyric struct failed!!!\n", __FUNCTION__);
        return NULL;
    }

    memset(lrc_p, 0, sizeof(lyric_t));

    lrc_p->lyric_list_head = lyric_list_node_create(0, NULL, 0);

    ret = lyric_parser(lrc_p, lrc_name);
    if (ret != 0) {
        LP_ERROR("%s: lrc parser failed ret:%d!!!\n", __FUNCTION__, ret);
        lyric_uninit((lrc_p));
        return NULL;
    }

    return lrc_p;
}

void lyric_uninit(lyric_t *lrc_p)
{
    if (lrc_p != NULL) {
        if (lrc_p->author != NULL) {
            SCI_FREE(lrc_p->author);
            lrc_p->author = NULL;
        }
        if (lrc_p->by != NULL) {
            SCI_FREE(lrc_p->by);
            lrc_p->by = NULL;
        }
        if (lrc_p->album != NULL) {
            SCI_FREE(lrc_p->album);
            lrc_p->album = NULL;
        }
        if (lrc_p->artist != NULL) {
            SCI_FREE(lrc_p->artist);
            lrc_p->artist = NULL;
        }
        if (lrc_p->re != NULL) {
            SCI_FREE(lrc_p->re);
            lrc_p->re = NULL;
        }
        if (lrc_p->title != NULL) {
            SCI_FREE(lrc_p->title);
            lrc_p->title = NULL;
        }
        if (lrc_p->version != NULL) {
            SCI_FREE(lrc_p->version);
            lrc_p->version = NULL;
        }
        lrc_p->offset = 0;
        lyric_list_destroy(lrc_p->lyric_list_head);

        SCI_FREE(lrc_p);
        lrc_p = NULL;
    }
}

int lyric_get_node_num(lyric_t *lrc_p)
{
    #if 0
    int num = 0;
    if (lrc_p != NULL && lrc_p->lyric_list_head != NULL) {
        lyric_content_node *current = lrc_p->lyric_list_head->next; // first node
        while (current != NULL) {
            num++;
            current = current->next;
        }
    }
    return num;
    #else
    return lrc_p != NULL ? lrc_p->lyric_num : 0;
    #endif
}

lyric_content_node *lyric_get_node(lyric_t *lrc_p, long milsec)
{
    if (lrc_p != NULL && lrc_p->lyric_list_head != NULL) {
        lyric_content_node *current = lrc_p->lyric_list_head; // head node, not first node
        while (current != NULL) {
            if (current->next == NULL) {
                if (current->timems > 0) {
                    return current;
                }
                break;
            } else {
                if (current->next->timems > milsec) {
                    if (current->timems == 0) {
                        return current->next;
                    }
                    return current;
                } else {
                    current = current->next;
                }
            }
        }
    }
    return NULL;
}

int lyric_get_node_index(lyric_t *lrc_p, long milsec)
{
    int count = -1;
    if (lrc_p != NULL && lrc_p->lyric_list_head != NULL) {
        lyric_content_node *current = lrc_p->lyric_list_head; // head node, not first node
        while (current != NULL) {
            if (current->next == NULL) {
                if (current->timems > 0) {
                    return count;
                }
                break;
            } else {
                if (current->next->timems > milsec) {
                    if (current->timems == 0) {
                        return count + 1;
                    }
                    return count;
                } else {
                    current = current->next;
                    count++;
                }
            }
        }
    }
    return count;
}

lyric_content_node *lyric_get_node_by_index(lyric_t *lrc_p, int index)
{
    lyric_content_node *current = NULL;
    if (lrc_p != NULL && lrc_p->lyric_list_head != NULL) {
        int count = 0;
        current = lrc_p->lyric_list_head->next; // first node
        while (current != NULL && count < index) {
            current = current->next;
            count++;
        }
    }
    return current;
}

int lyric_get_all_node_content_len(lyric_t *lrc_p)
{
    int content_len = 0;
    if (lrc_p != NULL && lrc_p->lyric_list_head != NULL) {
        lyric_content_node *current = lrc_p->lyric_list_head->next; // first node
        while (current != NULL) {
            content_len += current->content_len;
            current = current->next;
        }
    }
    return content_len;
}

char *lyric_get_all_node_content(lyric_t *lrc_p)
{
    char *content = NULL;
    if (lrc_p != NULL && lrc_p->lyric_list_head != NULL) {
        int content_len = 0;
        lyric_content_node *current = lrc_p->lyric_list_head->next; // first node
        while (current != NULL) {
            content_len += current->content_len + 1; // \n
            current = current->next;
        }

        if (content_len > 0) {
            int copy_len = 0;
            content = SCI_ALLOC_APPZ(content_len + 1);
            memset(content, 0, content_len + 1);
            current = lrc_p->lyric_list_head->next; // first node           
            while (current != NULL) {
                if (current->content != NULL && current->content_len > 0) {
                    if (current->next != NULL) {
                        sprintf(&content[copy_len], "%s\n", current->content);
                        copy_len += current->content_len + 1;
                    } else {
                        sprintf(&content[copy_len], "%s", current->content);
                        copy_len += current->content_len + 1;
                    }                    
                }
                current = current->next;
            }
            return content;
        }
    }
    return NULL;
}

