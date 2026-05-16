import cv2
import time
import matplotlib.pyplot as plt
from PIL import Image

def find_cameras():
    for i in range(10):
        cap = cv2.VideoCapture(i, cv2.CAP_DSHOW)
        print(f"Camera opened: {cap.isOpened()}")
        cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1920)    # stored, no reconfiguration
        cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 1080)   # both set → reconfiguration at new size
        cap.set(cv2.CAP_PROP_FOURCC,               # reconfigures again with codec + dims
        cv2.VideoWriter.fourcc('M','J','P','G'))
        cap.set(cv2.CAP_PROP_FPS, 30)             # reconfigures again with target FPS
        #print(f"Backend: {cap.getBackendName()}")
        
        if cap.isOpened():
            # Give it time to initialize
            time.sleep(1)

            ret, frame = cap.read()
            if ret:
                print(f"✓ Camera found at index {i}")
                print(f"  Resolution: {int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))}x{int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))}")

                result = Image.fromarray(frame.astype('uint8'))
                plt.imshow(result, cmap='gray')
                plt.axis('off')
                plt.show()
            
            cap.release()
        #else:
            #print(f"✗ Camera index {i} not available")

find_cameras()
