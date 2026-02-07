extends Node

var loaded_data: Array[GDTelemetrySnapshot]
var last_i := 0
var accum := 0.0

func _ready() -> void:
	if !$ACTelemetry.load_session_data("res://lap.bin").is_empty(): return
	loaded_data = $ACTelemetry.get_loaded_session_lap_data(0)

func _process(delta: float) -> void:
	if !loaded_data: return
	
	accum += delta
	if accum < 0.02: return
	
	accum -= 0.02
	
	var snapshot = loaded_data[last_i]
	$Label.text = str(int(snapshot.physics_speedKmh))
	#$Label.text = "%0.2f" % snapshot.graphic_normalizedCarPosition
	if last_i < loaded_data.size()-1:
		last_i += 1














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
