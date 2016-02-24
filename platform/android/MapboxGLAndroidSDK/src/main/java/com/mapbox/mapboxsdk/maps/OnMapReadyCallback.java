package com.mapbox.mapboxsdk.maps;

import android.support.annotation.NonNull;

/**
 * Interface definition for a callback to be invoked when the map is ready to be used.
 * <p>
 * Once an instance of this interface is set on a {@link MapFragment} or {@link MapView} object,
 * the onMapReady(MapboxMap) method is triggered when the map is ready to be used and provides a non-null instance of {@link MapboxMap}.
 * </p>
 */
public interface OnMapReadyCallback {

    /**
     * Called when the map is ready to be used.
     *
     * @param mapboxMap A non-null instance of a MapboxMap associated with the {@link MapFragment}
     *                  or {@link MapView} that defines the callback.
     */
    void onMapReady(@NonNull MapboxMap mapboxMap);
}
