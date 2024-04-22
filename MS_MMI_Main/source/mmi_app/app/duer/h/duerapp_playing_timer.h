// Copyright (2021) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_playing_timer.h
 * Auth: Li Chang (changli@baidu.com)
 * Desc: Try playing timer APIs.
 */

#ifndef __DUERAPP_PLAYING_TIMER_H__
#define __DUERAPP_PLAYING_TIMER_H__

int duer_get_try_playing_max_cnt();


void duer_set_try_playing_max_cnt(int max_cnt);


int duer_try_playing_timer_start();


int duer_try_playing_timer_stop();


int duer_try_playing_timer_create(int duration, int *playing_cnt);


int duer_try_playing_timer_delete();

#endif // __DUERAPP_PLAYING_TIMER_H__
