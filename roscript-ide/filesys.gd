extends Control

@onready var tree = $Tree
var md=false

func _ready():
	tree.clear()
	tree.item_activated.connect(_on_item_activated)

func _process(delta: float) -> void:
	save()

func scan_dir(path: String, parent: TreeItem):
	md=false
	var dir = DirAccess.open(path)
	parent.set_text(0,path.get_file())
	if dir:
		dir.list_dir_begin()
		var file_name = dir.get_next()
		while file_name != "":
			if file_name.begins_with("."):
				file_name = dir.get_next()
				continue
			var item = tree.create_item(parent)
			item.set_text(0, file_name)
			var full_path = path + "/" + file_name
			item.set_metadata(0, full_path)
			if dir.current_is_dir():
				scan_dir(full_path, item)
			file_name = dir.get_next()
		dir.list_dir_end()

func _on_item_activated():
	var item = tree.get_selected()
	if item:
		var path = item.get_metadata(0)
		if not DirAccess.dir_exists_absolute(path):  # skip folders
			var file = FileAccess.open(path, FileAccess.READ)
			if file:
				var contents = file.get_as_text()
				get_parent().get_child(2).text = contents  # show in your text editor
				get_parent().get_child(2).set_meta("current_path", path)
				md=true

func save():
	if md:
		var path = get_parent().get_child(2).get_meta("current_path")
		if path:
			var file = FileAccess.open(path, FileAccess.WRITE)
			if file:
				file.store_string(get_parent().get_child(2).text)
				file.flush()
				print("Saved to ", path)
