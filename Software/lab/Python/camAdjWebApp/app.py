from flask import Flask, render_template, jsonify, request, send_from_directory
from image_capture import CameraCapture, FakeCapture
from stream import Stream
from processing import *

import time
import os

app = Flask(
    __name__,
    static_folder="client_app/static",
    static_url_path="/static",
    template_folder="client_app/templates",
)

#camera = CameraCapture(camera_index=0, capture_interval=0.5)
camera = FakeCapture()

processing_store = [
    SizePositionRotateSkewFilter(id="SPR", type="Filter", name="Size, Position, Rotate"),
    EdgeFilter(id="EDG", type="Filter", name="Gaussian Blur and Canny Edge Detection"),
    ConvertToJpgProcessing(id="JPG", type="Converter", name="Convert to JPEG"),
    HoughLinesFeatureDetector(id="HGL", type="Analyzer", name="Hough transform line detection"),
]

stream_store = [
    Stream(id=1, name="Captured Image from Camera", status="New"),
    Stream(id=2, name="Blured Image", status="New"),
    Stream(id=3, name="Line Profile Image", status="New"),
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

    normalizer = processing_store[0]

    if request.method == "GET":
        # Return current values
        return jsonify(
            {
                "x": normalizer.x,
                "y": normalizer.y,
                "width": normalizer.width,
                "height": normalizer.height,
                "canny_upper_threshold": 50,
                "canny_lower_threshold": 100,
            }
        )

    if request.method == "POST":
        data = request.json

        # roi_x_str = data.get('x')
        camera_roi_x = data.get(
            "x"
        )  # int(roi_x_str) if roi_x_str is not None else None

        # roi_y_str = data.get('y')
        camera_roi_y = data.get(
            "y"
        )  # int(roi_y_str) if roi_y_str is not None else None

        # roi_w_str = data.get('width')
        camera_roi_w = data.get(
            "width"
        )  # int(roi_w_str) if roi_w_str is not None else None

        # roi_h_str = data.get('height')
        camera_roi_h = data.get(
            "height"
        )  # int(roi_h_str) if roi_h_str is not None else None

        normalizer.x = camera_roi_x
        normalizer.y = camera_roi_y
        normalizer.width = camera_roi_w
        normalizer.height = camera_roi_h

        return jsonify(
            {
                "x": normalizer.x,
                "y": normalizer.y,
                "width": normalizer.width,
                "height": normalizer.height,
                "canny_upper_threshold": 50,
                "canny_lower_threshold": 100,
            }
        )


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
            frame = camera.get_frame()

            sizePosRtnFilter = next((p for p in processing_store if p.id == "SPR"), None)
            frame = sizePosRtnFilter.run(frame) if sizePosRtnFilter else frame

            edgeFilter = next((p for p in processing_store if p.id == "EDG"), None)


            # TODO: Create two streams, one for the live image and one for the feature detection;
            #       but not here in video_feed

            if video_show_line_profile:
                frame = edgeFilter.run(frame) if edgeFilter else frame


            jpgConverter = next((p for p in processing_store if p.id == "JPG"), None)
            frame = jpgConverter.run(frame) if jpgConverter else frame

            if frame:
                yield (
                    b"--frame\r\n" b"Content-Type: image/jpeg\r\n\r\n" + frame + b"\r\n"
                )
                time.sleep(0.5)
            else:
                time.sleep(1)

    return app.response_class(
        generate(), mimetype="multipart/x-mixed-replace; boundary=frame"
    )


@app.route("/video_settings", methods=["GET", "POST"])
def video_settings():
    global video_show_line_profile

    if request.method == "GET":
        # Return current values
        return jsonify(
            {
                "enabled": video_show_line_profile,
            }
        )

    if request.method == "POST":
        data = request.json
        video_show_line_profile = data.get("enabled")

        return jsonify({"status": "ok", "enabled": video_show_line_profile})


if __name__ == "__main__":
    print("Starting camera...")
    camera.start()
    time.sleep(2)
    print("Camera started, starting Flask app...")
    try:
        #app.run(debug=False, host="0.0.0.0", port=5000, threaded=True)
        app.run(debug=True, port=5000, threaded=True)
    finally:
        camera.stop()
