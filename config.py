import os
basedir = os.path.abspath(os.path.dirname(__file__))


if os.environ.get('DATABASE_URL') is None:
	SQLALCHEMY_DATABASE_URI = 'sqlite:///' + os.path.join(basedir, 'api.db')

else:
	SQLALCHEMY_DATABASE_URI = os.environ['DATABASE_URL']

if os.environ.get('MONGODB_URI') is None:
	MONGODB_URI = 'mongodb://localhost:27017/'
else:
	MONGODB_URI = os.environ.get('MONGODB_URI')

SECRET_KEY = 'theapi2019#'

CSRF_ENABLED = True



