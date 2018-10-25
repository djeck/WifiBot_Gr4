import numpy as np
import cv2

def cbButton1 (x):
	cv2.imwrite("reference.jpg", frame)
    	cv2.imshow("captured", frame)
	print("Button 1 clicked");

# Open Stream
cap = cv2.VideoCapture("http://192.168.1.77:8080/?action=stream")

# Open Ref Image

# Add GUI
button_name_1 = 'Capture';
main_frame_name = 'frame';
cv2.namedWindow(main_frame_name, cv2.WINDOW_NORMAL);
cv2.createTrackbar(button_name_1, main_frame_name, 0, 1, cbButton1)

while(True):
    # Capture frame-by-frame
    ret, frame = cap.read()

    # Our operations on the frame come here
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    # Display the resulting frame
    stream_or_ref = cv2.getTrackbarPos(button_name_1, main_frame_name)

    # Switch views
    if stream_or_ref == 0:
	    cv2.imshow(main_frame_name,gray)
    else:
    	    cv2.imshow(main_frame_name, 0)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# When everything done, release the capture
cap.release()
cv2.destroyAllWindows()
