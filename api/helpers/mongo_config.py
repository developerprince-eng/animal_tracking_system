import os
from pymongo import MongoClient
from dotenv import load_dotenv
import config

APP_ROOT = os.path.join(os.path.dirname(__file__), '')   # refers to application_top
dotenv_path = os.path.join(APP_ROOT, '.env')
load_dotenv(dotenv_path)

mongo_uri = config.MONGODB_URI

class DBCONFIG():    
    client = MongoClient(mongo_uri)
