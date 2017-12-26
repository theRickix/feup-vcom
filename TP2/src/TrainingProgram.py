import cv2
import ObjectDefinitions
import os
from tkinter import filedialog
from tkinter import *
from os import *

# Training program to recognize various object from the AID (Aerial Image Database)

# Create window to get the directory where the images for training are
directory = filedialog.askdirectory()
# used to obtain all files and directories in a given root directory
pathSelectionInfo = walk(directory)

for root, dirs, files in pathSelectionInfo:
    if dirs:
        print('Please choose a directory without any sub folders.')
        break
    elif files:
        nFiles = ''
        while not isinstance(nFiles, int):
            try:
                nFiles = int(input('How many files would you like to use to train the learning algorithm? '))
            except TypeError:
                print('That is not a valid number. \n')
        print('Creating detector...')
        sift_detector = cv2.xfeatures2d.SIFT_create()
        for fileName in sorted(files):
            if nFiles < 1:
                print('Ended keypoints printing...')
                break
            elif fileName.lower().endswith('.jpg'):
                print('File: ' + fileName)
                img = cv2.imread(os.path.join(root, fileName))
                gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
                kp = sift_detector.detect(gray, None)
                img = cv2.drawKeypoints(gray, kp, None)
                cv2.imwrite('sift_detector_kp.jpg', img)
                nFiles -= 1
            else:
                print('The file: ' + fileName + ' is not a jpg image file. \n')
    else:
        print('No files found on directory: ' + root)