Set fso = CreateObject("Scripting.FileSystemObject")
Set f = fso.OpenTextFile( "Resource.h",1 )

Set Text = CreateObject("Scripting.Dictionary")
Set Value = CreateObject("Scripting.Dictionary")

nCount = 0

Set regEx = New RegExp   
regEx.Pattern = "#define[ \t]+(\w+)[ \t]+(\d+)"

While Not f.AtEndofStream
	LineContent = f.ReadLine
	Set Matchs = regEx.Execute( LineContent )
	
	If Matchs.Count > 0 Then
		Set match = Matchs(0)
		Set SubMatch = match.SubMatches
		Text.Add nCount,SubMatch( 0 )
		Value.Add nCount,SubMatch( 1 )
		nCount = nCount + 1
	End If 
Wend

f.Close

Set f = fso.OpenTextFile( "CtrlStatus_gen.cpp",2,True )

f.WriteLine( "//////////////////////////////////////////////////" )
f.WriteLine( "// CtrlStatus_gen.cpp")
f.WriteLine( "// AutoGenerated file")
f.WriteLine( "// A List for id and name pair list")
f.WriteLine( "" )
f.WriteLine( "#include " & Chr(34) & "stdafx.h" & Chr(34) )
f.Write( "const int gnIdCount = " )
f.Write( nCount )
f.Write( ";" )
f.WriteLine( "" )
f.WriteLine( "const CString gstrCtrlName[] = " )
f.WriteLine( "{" )

For counter = 0 To nCount - 1
	f.WriteLine( "    " & Chr(34) & Text.Item( counter ) & Chr(34) & "," )
Next

f.WriteLIne( "};" )
f.WriteLine( "" )
f.WriteLine( "const int gnCtrlId[] = " )
f.WriteLine( "{" )

For counter = 0 to nCount - 1
	f.WriteLine( "    " & Value.Item( counter ) & "," )
Next

f.WriteLine( "};" )

f.WriteLine( "int FindIdByName( CString& strName )" )
f.WriteLine( "{" )
f.WriteLine( "    for( int i=0;i<gnIdCount;i++ )" ) 
f.WriteLine( "    {" )
f.WriteLine( "        if( strName == gstrCtrlName[i] )" )
f.WriteLine( "        {" )
f.WriteLine( "            return gnCtrlId[i];" )
f.WriteLine( "        }" )
f.WriteLine( "    }" )
f.WriteLine( "" )
f.WriteLine( "   return -1;" )
f.WriteLine( "}" )

f.Close

