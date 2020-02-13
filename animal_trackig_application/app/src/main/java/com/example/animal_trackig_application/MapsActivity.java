package com.example.animal_trackig_application;

import android.os.Bundle;

import androidx.fragment.app.FragmentActivity;

import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.MarkerOptions;

import  java.util.List;

import retrofit2.Callback;
import retrofit2.Response;
import retrofit2.Retrofit;
import retrofit2.converter.gson.GsonConverterFactory;
import retrofit2.Call;

public class MapsActivity extends FragmentActivity implements OnMapReadyCallback {

    private GoogleMap mMap;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_maps);
        // Obtain the SupportMapFragment and get notified when the map is ready to be used.
        SupportMapFragment mapFragment = (SupportMapFragment) getSupportFragmentManager()
                .findFragmentById(R.id.map);
        mapFragment.getMapAsync(this);
    }


    @Override
    public void onMapReady(GoogleMap googleMap) {
        mMap = googleMap;


        Retrofit retrofit = new Retrofit.Builder()
                .baseUrl("https://animal-trackig-api.herokuapp.com/")
                .addConverterFactory(GsonConverterFactory.create())
                .build();

        JsonPlaceHolder jsonPlaceHolder = retrofit.create(JsonPlaceHolder.class);

        Call<List<Location>> call = jsonPlaceHolder.getLocations();

        call.enqueue(new Callback<List<Location>>() {
            @Override
            public void onResponse(Call<List<Location>> call, Response<List<Location>> response) {
                if(!response.isSuccessful()){
                    return;
                }

                List<Location> locations = response.body();

                for (Location location: locations){
                    String str_lat = location.getLatitude();
                    String str_longt = location.getLongitude();
                    float lat = Float.parseFloat(str_lat);
                    float longt = Float.parseFloat(str_longt);
                    mMap.addMarker(new MarkerOptions().position(new LatLng(lat, longt)).title("Child Marker"));
                }






            }

            @Override
            public void onFailure(Call<List<Location>> call, Throwable t) {
                t.getMessage();
            }
        });


    }

}
