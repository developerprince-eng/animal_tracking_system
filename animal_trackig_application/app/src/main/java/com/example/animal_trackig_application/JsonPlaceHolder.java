package com.example.animal_trackig_application;



import  java.util.List;
import retrofit2.http.GET;
import retrofit2.Call;


public interface JsonPlaceHolder {

    @GET ("track")
    Call<List<Location>> getLocations();
}