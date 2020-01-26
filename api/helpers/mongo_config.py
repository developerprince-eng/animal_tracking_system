import os
from pymongo import MongoClient
from dotenv import load_dotenv

APP_ROOT = os.path.join(os.path.dirname(__file__), '')   # refers to application_top
dotenv_path = os.path.join(APP_ROOT, '.env')
load_dotenv(dotenv_path)

mongo_uri = os.getenv('MONGODB_URI') or os.environ.get('MONGODB_URI') 

class DBCONFIG():    
    client = MongoClient(mongo_uri, retryWrites = False)
