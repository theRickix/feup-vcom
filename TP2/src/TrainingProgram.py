import cv2
import ObjectDefinitions
import os
from tkinter import filedialog
from tkinter import *
from imutils import paths
from os import listdir
from sklearn.neighbors import KNeighborsClassifier
from sklearn.cross_validation import train_test_split


def image_to_feature_vector(image, size=(32, 32)):
    return cv2.resize(image, size).flatten()


def extract_color_histogram(image, bins=(8, 8, 8)):
    hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
    hist = cv2.calcHist([hsv], [0, 1, 2], None, bins,
                        [0, 180, 0, 256, 0, 256])

    cv2.normalize(hist, hist)

    return hist.flatten()

# Training program to recognize various object from the AID (Aerial Image Database)

# Create window to get the directory where the images for training are
print('Waiting selection of AID...')
rootDir = filedialog.askdirectory(title = 'Navigate to the directory containing the AID')

# used to obtain all files and directories in a given root directory
# pathSelectionInfo = walk(directory)

# List all subdirectories and initialize vars
rawImages = []
features = []
labels = []

subdirs = sorted(listdir(rootDir))

if subdirs:
    for directory in subdirs:
        imagePaths = os.listdir(os.path.join(rootDir, directory))
        for (i, imagePath) in enumerate(imagePaths):
            image = cv2.imread(os.path.join(rootDir, directory, imagePath))
            label = directory
            pixels = image_to_feature_vector(image)
            hist = extract_color_histogram(image)

            rawImages.append(pixels)
            features.append(hist)
            labels.append(label)
        print("Directory '", directory, "' loaded.")

(trainRI, testRI, trainRL, testRL) = train_test_split(
    rawImages, labels, test_size=0.25, random_state=42)
(trainFeat, testFeat, trainLabels, testLabels) = train_test_split(
    features, labels, test_size=0.25, random_state=42)

model = KNeighborsClassifier(2)
model.fit(trainRI, trainRL)
acc = model.score(testRI, testRL)
print("[INFO] raw pixel accuracy: {:.2f}%".format(acc * 100))
