extends Node

func _ready() -> void:
	await get_tree().create_timer(1).timeout
	print($ACTelemetry.load_session_data("res://lap.bin"))
	print($ACTelemetry.get_loaded_session_lap_count())
	print($ACTelemetry.get_loaded_session_sample_interval())
	#print($ACTelemetry.get_loaded_session_lap_data())

#var timer: Timer
#
#func _ready() -> void:
	#timer = Timer.new()
	#timer.wait_time = 1.0
	#timer.autostart = true
	#timer.timeout.connect(_on_timer_timeout)
	#add_child(timer)
#
#func _on_timer_timeout():
	#if !$ACTelemetry.is_connected_to_ac():
		#$ACTelemetry.connect_to_ac()
		#if $ACTelemetry.is_connected_to_ac():
			#print("Connected to AC!")
			#$ACTelemetry.start_logging()
			#
			#timer.autostart = false
			#timer.stop()
#
#func _process(_delta):
	#if $ACTelemetry.is_connected_to_ac():
		#if Input.is_action_just_pressed("ui_cancel"):
			#print($ACTelemetry.finish_logging("res://lap.bin"))
			#get_tree().quit()
