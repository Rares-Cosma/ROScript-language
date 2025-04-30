extends Node2D

@onready var fisier=$"Fișier"
@onready var run=$"Run"

func _ready():
	var popup = fisier.get_popup()
	popup.add_item("Import")
	popup.id_pressed.connect(_on_item_selected)
	var popup2 = run.get_popup()
	popup2.add_item("Run .ros file")
	popup2.id_pressed.connect(_on_item_selected_run)

func run_command(command: String):
	var path = "user://temp.bat"
	var file = FileAccess.open(path, FileAccess.WRITE)
	file.store_line(command)
	file.flush()
	file.close()
	OS.shell_open(ProjectSettings.globalize_path(path))

func _on_item_selected(id):
	var item_name = fisier.get_popup().get_item_text(id)
	match item_name:
		"Import":
			$NewProject.current_dir="/"
			$NewProject.show()
func _on_item_selected_run(id):
	var item_name = run.get_popup().get_item_text(id)
	match item_name:
		"Run .ros file":
			if $Control.md:
				if $CodeInterface.get_meta("current_path").ends_with(".ros"):
					run_command("cd ../interpreter/bin && .\\ros.exe "+$CodeInterface.get_meta("current_path")+" && pause")

func _on_new_project_dir_selected(dir: String) -> void:
	$Control/Tree.clear()
	$Control.scan_dir(dir,$Control/Tree.create_item())
