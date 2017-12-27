import cv2
import ObjectDefinitions
import os
from tkinter import filedialog
from tkinter import *
from os import listdir
from keras import Sequential


# Training program to recognize various object from the AID (Aerial Image Database)

# Create window to get the directory where the images for training are
print('Waiting selection of AID...')
rootDir = filedialog.askdirectory(title = 'Navigate to the directory containing the AID')

# Ask for image size details
imageParams = (600, 600)
answer = ''
while not answer.lower() == 'y' or answer.lower() == 'n':
    answer = input('Use default image size parameters? [y/n] ')

if answer.lower() == 'n':
    width = ''
    height = ''
    while not (isinstance(width, int) or width < 0) :
        if width:
            print('Image dimensions cannot be negative. \n')
        try:
            width = int(input('Image width: '))
        except TypeError:
            print('Invalid width input. \n')
    while not (isinstance(height, int) or height < 0 
        if height:
            print('Image dimensions cannot be negative. \n')
        try:
            height = int(input('Image height: '))
        except TypeError:
            print('Invalid height input. \n')
    imageParams = (width, height)

# Create keras model
model = Sequential()
numberOfDefinitions = len(RealWorldObjectType)


# used to obtain all files and directories in a given root directory
# pathSelectionInfo = walk(directory)

# List all subdirectories
subdirs = sorted(listdir(directory))
if subdirs:
    for directory in subdirs:


'''
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
        break
'''