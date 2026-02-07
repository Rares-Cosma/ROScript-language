extends CodeEdit

func _ready():
	var highlighter = CodeHighlighter.new()
	
	var kwds=["daca"]
	for i in kwds:
		highlighter.add_keyword_color(i, Color(0.0, 0.536, 0.201))
	
	highlighter.symbol_color = Color(0.531, 0.381, 0.143) # all {}, (), etc. in red
	
	highlighter.add_color_region("\"", "\"", Color(0.875, 0.816, 0.561))
	highlighter.add_color_region("//", "", Color(0.393, 0.393, 0.393), true)
	
	syntax_highlighter = highlighter
