extends Node2D

@onready var fisier=$"Fișier"

func _ready():
	var popup = fisier.get_popup()
	popup.add_item("Import")
	popup.id_pressed.connect(_on_item_selected)
	

func _on_item_selected(id):
	var item_name = fisier.get_popup().get_item_text(id)
	match item_name:
		"Import":
			$NewProject.current_dir="/"
			$NewProject.show()

func _on_new_project_dir_selected(dir: String) -> void:
	$Control/Tree.clear()
	$Control.scan_dir(dir,$Control/Tree.create_item())
