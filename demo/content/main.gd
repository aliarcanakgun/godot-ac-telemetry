extends Node

var timer: Timer

func _ready() -> void:
	# use a dedicated timer for retries
	timer = Timer.new()
	timer.wait_time = 1.0
	timer.autostart = true
	timer.timeout.connect(_on_timer_timeout)
	add_child(timer)

func _on_timer_timeout():
	if !$ACTelemetry.is_connected_to_ac():
		$ACTelemetry.connect_to_ac()
		if $ACTelemetry.is_connected_to_ac():
			print("Connected to AC!")
			$ACTelemetry.start_logging()
			
			timer.autostart = false
			timer.stop()

func _process(_delta):
	if $ACTelemetry.is_connected_to_ac():
		print("Speed: %.2f" % $ACTelemetry.get_speed())
		
		if Input.is_action_just_pressed("ui_cancel"):
			print($ACTelemetry.finish_logging())
