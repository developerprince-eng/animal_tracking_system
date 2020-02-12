
import os
from flask import flash, redirect, render_template, url_for, request, Flask, jsonify, send_file
import json
from datetime import datetime 
import time
from werkzeug.security import generate_password_hash
from werkzeug.utils import secure_filename
from . import app
ALLOWED_EXTENSIONS = set(['txt', 'pdf', 'png', 'jpg', 'jpeg', 'gif'])
from . import mongoconfig
def allowed_file(filename):
	return '.' in filename and filename.rsplit('.', 1)[1].lower() in ALLOWED_EXTENSIONS



#BAD REQUEST
@app.errorhandler(400)
def resource_error(e):
	return render_template('400.html'), 400

#FORBIDDEN ERROR
@app.errorhandler(403)
def forbidden(e):
	return render_template('403.html'), 403

#NOT ALLOWED ERROR
@app.errorhandler(405)
def method_not_allowed(e):
	return render_template('405.html'), 405

#NOT FOUND ERROR
@app.errorhandler(404)
def page_not_found(e):
	return render_template('404.html'), 404

#SERVER INTERNAL ERROR
@app.errorhandler(500)
def server_error(e):
	return render_template('500.html'), 500

#BAD GATEWAY
@app.errorhandler(502)
def server_error_bad_gateway(e):
	return render_template('502.html'), 502

#GATEWAY TIMEOUT
@app.errorhandler(504)
def server_error_gateway_timeout(e):
	return render_template('504.html'), 504

#HOME PAGE INDEX
@app.route("/", methods=['GET'])
def index():
	resp = jsonify({'message' : 'The API IS READY'})
	return resp

@app.route("/track", methods=['GET', 'POST'])
def track():
	if request.method == 'POST':
		temperature = request.values.get('temperature')
		heart_rate = request.values.get('heart_rate')
		longitude = request.values.get('longitude') 
		latitude = request.values.get('latitude') 
		date_time = str(datetime.now())
		db = mongoconfig['animal_track']
		collection = db['locations']
		loc_data= {
            "temperature" : temperature,
            "heart_rate" : heart_rate,
            "longitude" : longitude,
            "latitude" : latitude,
			"created_time" : date_time
        }
		collection.insert_one(loc_data)
		resp = jsonify({'status' :'success'})
		resp.status_code = 201
		return resp
		
	else:
		db = mongoconfig['animal_track']
		collection = db['locations']
		documents = collection.find()
		response = []
		for document in documents:
			document['_id'] = str(document['_id'])
			response.append(document)
		return json.dumps(response)

