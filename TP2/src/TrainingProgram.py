import cv2
from os import walk
import ObjectDefinitions
from tkinter import filedialog
from tkinter import *

# Create window to get the directory where the images for training are
pathSelectionWindow = Tk()
directory = filedialog.askdirectory()
pathSelectionInfo = walk(directory)
pathSelectionWindow.destroy()

for root, dirs, files in pathSelectionInfo:
    if files:
        print('No files found on directory')
    else:
        print('How many files would you like to use to train the learning algorithm? ')
        nFiles = input()
        while not nFiles.is_integer():
            print('\nPlease insert a number: ')
            nFiles = input()
        sift_detector = cv2.xfeatures2d.SIFT_create()
        for fileName in files:
            if fileName.lower.endswith('.jpg'):
                img = cv2.imread(os.path.join(pathSelectionWindow.directory, fileName))
                gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
                kp = sift_detector.detect(gray, None)
                img = cv2.drawKeypoints(gray, kp)
                cv2.imwrite('sift_detector_kp.jpg', img)
            else:
                print('The file: ' + fileName + ' is not a jpg image file. \n')