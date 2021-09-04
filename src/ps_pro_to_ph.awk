BEGIN { 
	print "// DO NOT CHANGE THIS FILE. THIS FILE IS GENERATED FROM pstoedit.pro "; 
	print "// You can get pstoedit.pro from the author of pstoedit"; 
	print "// pstoedit.pro contains a lot more comments and explanations than pstoedit.ph"; 
	print "const char * const PS_prologue[] = "; print "{"; 
} 
/^%\/\//      { gsub(/%/,""); print $0; next;}	
/^[ \t]*%/      { next; }
/^[ \t]*$$/      { next; }
{ gsub(/^[ \t]*/,""); 
  gsub(/\\/,"\\\\"); 
  gsub(/"/,"\\\""); 
  print " \" " $0 " \","; } 
END   { print "nullptr"; print "};" } 
