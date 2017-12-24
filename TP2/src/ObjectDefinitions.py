from enum import Enum
class RealWorldObjectType(Enum):
    AIRPORT = 0,
    BARELAND = 1,
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
    PLAYGROUND = 18
    PORT = 19,
    RAILWAY_STATION = 20,
    RESORT = 21,
    RIVER = 22,
    SCHOOL = 23,
    SPARSE_RESIDENTIAL = 24,
    SQUARE = 25,
    STADIUM = 26,
    STORAGE_TANKS = 27,
    VIADUCT = 28

class RealObject:
    def __init__(self, real_obj_type):
        if not isinstance(real_obj_type, RealWorldObjectType):
            print("Object type is not of RealWorldObjectType")
            del self
        else:
            self.real_object_type = real_obj_type