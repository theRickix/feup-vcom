# Local imports
from ObjectDefinitions import RealWorldObjectType

# Keras imports
from keras.preprocessing.image import img_to_array
from keras.models import load_model

# Numpy import
import numpy

# OpenCV import
import cv2

# Imutils imports
import imutils

# TKInter imports
from tkinter import filedialog
from tkinter import Tk
Tk().withdraw()

answer = 'y'

print('[INFO] Waiting model selection...')
modelFile = filedialog.askopenfilename(title = 'Choose the keras model')
while isinstance(modelFile, tuple):
    closeAnswer = input('No model file chosen. Do you wish to quit? (y/n) ')
    if closeAnswer.lower() == 'n':
        print('[INFO] Waiting model selection...')
        modelFile = filedialog.askopenfilename(title = 'Choose the keras model')
    elif closeAnswer.lower() == 'y':
        print('[INFO] Exiting program.')
        exit()
    else:
        print('[INFO] Invalid option. Retrying...')

print('[INFO] Waiting testing image selection...')
imgFile = filedialog.askopenfilename(title = 'Choose the image to test')
while isinstance(imgFile, tuple):
    closeAnswer = input('No image file chosen. Do you wish to quit? (y/n) ')
    if closeAnswer.lower() == 'n':
        print('[INFO] Waiting testing image selection...')
        imgFile = filedialog.askopenfilename(title = 'Choose the image to test')
    elif closeAnswer.lower() == 'y':
        print('[INFO] Exiting program.')
        exit()
    else:
        print('[INFO] Invalid option. Retrying...')

# Load images
image = cv2.imread(imgFile)
# Copy original image
imOrig = image.copy()
 
# Image preprocessing
image = cv2.resize(image, (28, 28))
image = image.astype('float') / 255.0
image = img_to_array(image)
image = numpy.expand_dims(image, axis=0)

# Load Keras Neural Network Model
print("[INFO] Loading Keras Model...")
model = load_model(modelFile)

# Get all probabilities on the image (tuple)
realWorldObjProbabilities = model.predict(image)[0]
max_prob = -1
label = -1
for i in range(len(realWorldObjProbabilities)):
    if realWorldObjProbabilities[i] > max_prob:
        max_prob = realWorldObjProbabilities[i]
        label = i

# Build the label with the information
label = RealWorldObjectType(label).name
label = "{}: {:.2f}%".format(label, max_prob * 100)
 
# Draw the label using the original image
output = imutils.resize(imOrig, width=600)
cv2.putText(output, label, (10, 25),  cv2.FONT_HERSHEY_DUPLEX,
	0.7, (244, 134, 66), 2)
 
# Open a new window with the output image
cv2.imshow("Output", output)
cv2.imwrite("Prediction.jpg", output)
cv2.waitKey(0)