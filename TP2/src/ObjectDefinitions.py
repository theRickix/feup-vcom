# Enums
from enum import IntEnum

# Keras imports
from keras import Sequential
from keras.layers.convolutional import Conv2D
from keras.layers.convolutional import MaxPooling2D
from keras.layers.core import Activation
from keras.layers.core import Flatten
from keras.layers.core import Dense
from keras import backend as K

class RealWorldObjectType(IntEnum):
    AIRPORT = 0,
    BARE_LAND = 1,
    BASEBALL_FIELD = 2,
    BEACH = 3,
    BRIDGE = 4,
    CENTER = 5,
    CHURCH = 6,
    COMMERCIAL = 7,
    DENSE_RESIDENTIAL = 8,
    DESERT = 9,
    FARMLAND = 10,
    FOREST = 11,
    INDUSTRIAL = 12,
    MEADOW = 13,
    MEDIUM_RESIDENTIAL = 14,
    MOUNTAIN = 15,
    PARK = 16,
    PARKING = 17,
    PLAYGROUND = 18,
    POND = 19,
    PORT = 20,
    RAILWAY_STATION = 21,
    RESORT = 22,
    RIVER = 23,
    SCHOOL = 24,
    SPARSE_RESIDENTIAL = 25,
    SQUARE = 26,
    STADIUM = 27,
    STORAGE_TANKS = 28,
    VIADUCT = 29

EPOCHS = 35
INIT_LEARN_RATE = 1e-3
BATCH_SIZE = 32

class LeNetModelBuilder:
    @staticmethod
    def buildModel(width, height, depth, classes):
        # Create keras model
        model = Sequential()
        inputShape = (width, height, depth)
        if K.image_data_format() == 'channels_first':
            inputShape = (depth, width, height)

        # Add first set of layers
        model.add(Conv2D(20, (5, 5), padding = 'same', input_shape = inputShape))
        model.add(Activation('relu'))
        model.add(MaxPooling2D(pool_size=(2, 2), strides=(2, 2)))

        # Add second set of layers
        model.add(Conv2D(50, (5, 5), padding = 'same'))
        model.add(Activation('relu'))
        model.add(MaxPooling2D(pool_size=(2, 2), strides=(2, 2)))

        model.add(Flatten())
        model.add(Dense(500))
        model.add(Activation('relu'))

        model.add(Dense(classes))
        model.add(Activation('softmax'))
        return model
