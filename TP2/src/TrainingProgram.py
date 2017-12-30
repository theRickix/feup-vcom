# Training program to recognize various object from the AID (Aerial Image Database)
# Local imports
from ObjectDefinitions import *

# OpenCV imports
import cv2

# OS imports
import os

# Random import
import random

# Regex import
import re

# TKInter imports
from tkinter import filedialog
from tkinter import Tk
Tk().withdraw()

# Keras imports
from keras.preprocessing.image import ImageDataGenerator
from keras.preprocessing.image import img_to_array
from keras.optimizers import Adam
from keras.utils import to_categorical
from sklearn.model_selection import train_test_split

# Tensorflow imports
import tensorflow as tf

from imutils import paths

import matplotlib
matplotlib.use('Agg')
from matplotlib import pyplot

import numpy

# Ask for which device to use to train the Keras Model
device_name = input('Please choose your preferred device to train the model (cpu/gpu): ')
while not device_name == 'gpu' and not device_name == 'cpu':
    device_name = input('Invalid device, please choose from cpu or gpu: ')

if device_name is 'gpu':
    device_name = '/gpu:0'
else:
    device_name = '/cpu:0'

# Create window to get the directory where the images for training are
print('[INFO] Waiting selection of AID root folder...')
rootDir = filedialog.askdirectory(title = 'Navigate to the directory containing the AID')
while isinstance(rootDir, tuple):
    closeAnswer = input('No AID directory chosen. Do you wish to quit? (y/n) ')
    if closeAnswer.lower() == 'n':
        print('[INFO] Waiting selection of AID root folder...')
        rootDir = filedialog.askdirectory(title = 'Navigate to the directory containing the AID')
    elif closeAnswer.lower() == 'y':
        print('[INFO] Exiting program.')
        exit()
    else:
        print('[INFO] Invalid option. Retrying...')

print('[INFO] Waiting selection of model output folder...')
modelWriteFolder = filedialog.askdirectory(title = 'Choose the directory to save the trained model')
while isinstance(modelWriteFolder, tuple):
    closeAnswer = input('No AID directory chosen. Do you wish to quit? (y/n) ')
    if closeAnswer.lower() == 'n':
        print('[INFO] Waiting selection of model output folder...')
        modelWriteFolder = filedialog.askdirectory(title = 'Choose the directory to save the trained model')
    elif closeAnswer.lower() == 'y':
        print('[INFO] Exiting program.')
        exit()
    else:
        print('[INFO] Invalid option. Retrying...')

print('[INFO] Image database Folder: ' + str(rootDir))
print('[INFO] Output Model Data Folder: ' + str(modelWriteFolder))

with tf.device(device_name):
    data = []
    labels = []
    imagePathsToUse = []

    subdirs = sorted(os.listdir(rootDir))

    # Load 95% of images in each folder to training set
    if subdirs:
        for directory in subdirs:
            imagePaths = os.listdir(os.path.join(rootDir, directory))
            for (i, imagePath) in enumerate(imagePaths):
                if (i / len(imagePaths)) > 0.95:
                    print("[INFO] Directory '", directory, "' with " + str(i) + "images to train.")
                    break
                imagePathsToUse.append(os.path.join(rootDir, directory,imagePath))

    randomSeed = 58
    random.seed(randomSeed)
    random.shuffle(imagePathsToUse)

    print('[INFO] Adding all data for usage in training (' + str(len(imagePathsToUse)) + ' images used).')
    for imagePath in imagePathsToUse:
        image = cv2.imread(imagePath)
        image = cv2.resize(image, (28, 28))
        image = img_to_array(image)
        data.append(image)

        realObjTypeStr = imagePath.split(os.path.sep)[-2]
        # Camel case spliting
        realObjTypeStr = re.findall(r'[A-Z](?:[a-z]+|[A-Z]*(?=[A-Z]|$))', realObjTypeStr)
        realObjTypeStr = '_'.join(realObjTypeStr).upper()

        label = RealWorldObjectType[realObjTypeStr].value
        labels.append(label)

    # Create numpy arrays for the data and labels, while scaling down the intensity 
    # of each pixel data from [0, 255] to [0, 1]
    data = numpy.array(data, dtype = 'float') / 255.0
    labels = numpy.array(labels)

    (trainX, testX, trainY, testY) = train_test_split(data, labels, test_size = 0.25, random_state = randomSeed)

    trainY = to_categorical(trainY, num_classes = len(RealWorldObjectType))
    testY = to_categorical(testY, num_classes = len(RealWorldObjectType))

    # construct the image generator for data augmentation
    aug = ImageDataGenerator(rotation_range=30, width_shift_range=0.1,
        height_shift_range=0.1, shear_range=0.2, zoom_range=0.2,
        horizontal_flip=True, fill_mode="nearest")

    # Build and compile the model
    print('[INFO] Model compiling in progress...')
    model = LeNetModelBuilder.buildModel(width = 28, height = 28, depth = 3, classes = len(RealWorldObjectType))
    opt = Adam(lr = INIT_LEARN_RATE, decay = INIT_LEARN_RATE / EPOCHS)
    model.compile(loss = 'binary_crossentropy', optimizer = opt, metrics = ['accuracy'])

    # Train the network
    print('[INFO] Network started training... (Verbose enabled)')
    H = model.fit_generator(aug.flow(trainX, trainY, batch_size = BATCH_SIZE), 
    validation_data = (testX, testY), steps_per_epoch = len(trainX) // BATCH_SIZE, epochs = EPOCHS, verbose = 1)

    # Save model to disk
    print('[INFO] Serializing network')
    model.save(os.path.join(modelWriteFolder, 'Model.kermod'))

    # Plots data and save plot image
    pyplot.style.use('ggplot')
    pyplot.figure()
    pyplot.plot(numpy.arange(0, EPOCHS), H.history['loss'], label = 'train_loss')
    pyplot.plot(numpy.arange(0, EPOCHS), H.history['val_loss'], label = 'val_loss')
    pyplot.plot(numpy.arange(0, EPOCHS), H.history['acc'], label = 'train_acc')
    pyplot.plot(numpy.arange(0, EPOCHS), H.history['val_acc'], label = 'val_acc')
    pyplot.title('Training data on AID (Loss and Accuracy)')
    pyplot.xlabel('Epoch #')
    pyplot.ylabel('Loss/Accuracy')
    pyplot.legend(loc = 'lower left')
    pyplot.savefig(os.path.join(modelWriteFolder, 'ModelTrain.jpg'))
    
