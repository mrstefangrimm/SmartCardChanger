from flask import Flask, render_template, jsonify, request, send_from_directory
from camera import CameraCapture
from processing import (
    ConvertToJpgProcessing,
    LineProfileProcessing,
    NormalizeDataProcessing,
    Processing,
)
import time
import os

app = Flask(
    __name__,
    static_folder="clientApp/static",
    static_url_path="/static",
    template_folder="clientApp/templates",
)

camera = CameraCapture(camera_index=0, capture_interval=1.0)

processing_store = [
    NormalizeDataProcessing(id=1, name="Apply ROI and stuff"),
    LineProfileProcessing(id=2, name="Convert to Line-Profile"),
    ConvertToJpgProcessing(id=3, name="Convert to JPEG"),
]

video_show_line_profile = False

camera_roi_x = 0
camera_roi_y = 0
camera_roi_w = 1920
camera_roi_h = 1080


@app.route("/")
def index():
    """Serve the main page."""
    return render_template("index.html")


@app.route("/camera_adjustments", methods=["GET", "POST"])
def camera_adjustments():
    global camera_roi_x
    global camera_roi_y
    global camera_roi_w
    global camera_roi_h

    if request.method == "POST":
        roi_x_str = request.form.get("roi_x")
        camera_roi_x = int(roi_x_str) if roi_x_str is not None else None

        roi_y_str = request.form.get("roi_y")
        camera_roi_y = int(roi_y_str) if roi_y_str is not None else None

        roi_w_str = request.form.get("roi_width")
        camera_roi_w = int(roi_w_str) if roi_w_str is not None else None

        roi_h_str = request.form.get("roi_height")
        camera_roi_h = int(roi_h_str) if roi_h_str is not None else None

    normalizer = processing_store[0]
    normalizer.x = camera_roi_x
    normalizer.y = camera_roi_y
    normalizer.width = camera_roi_w
    normalizer.height = camera_roi_h

    return render_template("index.html", submitted_text=camera_roi_x)


@app.get("/api/processings")
def get_processings():
    return jsonify([p.to_dict() for p in processing_store])


@app.route("/api/intersections")
def get_intersections():
    """API endpoint to get current intersections as JSON."""
    return jsonify(camera.get_intersections())


@app.route("/video_feed")
def video_feed():
    """Stream video with intersection points drawn."""

    def generate():
        while True:
            frame = camera.get_raw_frame()
            time.sleep(1)

            frame = processing_store[0].run(frame)

            if video_show_line_profile:
                frame = processing_store[1].run(frame)

            frame = processing_store[2].run(frame)

            if frame:
                yield (
                    b"--frame\r\n" b"Content-Type: image/jpeg\r\n\r\n" + frame + b"\r\n"
                )
            else:
                time.sleep(1)

    return app.response_class(
        generate(), mimetype="multipart/x-mixed-replace; boundary=frame"
    )


@app.post("/video_settings")
def video_settings():
    global video_show_line_profile

    data = request.get_json()
    video_show_line_profile = data.get("enabled")

    return jsonify({"status": "ok", "enabled": video_show_line_profile})


if __name__ == "__main__":
    print("Starting camera...")
    camera.start()
    time.sleep(2)
    print("Camera started, starting Flask app...")
    try:
        app.run(debug=False, host="0.0.0.0", port=5000, threaded=True)
    finally:
        camera.stop()
