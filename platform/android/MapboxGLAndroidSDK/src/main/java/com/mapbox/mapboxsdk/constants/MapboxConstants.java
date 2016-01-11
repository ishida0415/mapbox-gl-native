package com.mapbox.mapboxsdk.constants;

import java.util.Locale;

/**
 * MapboxConstants exposes Mapbox related constants
 */
public class MapboxConstants {

    /**
     * Default Locale for data processing (ex: String.toLowerCase(MAPBOX_LOCALE, "foo"))
     */
    public static final Locale MAPBOX_LOCALE = Locale.US;

    /**
     * Key used to store access token in AndroidManifest.xml
     */
    public static final String KEY_META_DATA_MANIFEST = "com.mapbox.AccessToken";

    /**
     * Default animation time
     */
    public static final int ANIMATION_DURATION = 300;

    /**
     * The currently supported minimum zoom level.
     */
    public static final double MINIMUM_ZOOM = 0f;

    /**
     * The currently supported maximum zoom level.
     */
    public static final double MAXIMUM_ZOOM = 25.0;

    /**
     * The currently supported maximum tilt value.
     */
    public static final double MAXIMUM_TILT = 60;

    /**
     * The currently supported minimum tilt value.
     */
    public static final double MINIMUM_TILT = 0;

    /**
     * The currently supported maximum direction
     */
    public static final double MAXIMUM_DIRECTION = 360;

    /**
     * The currently supported minimum direction
     */
    public static final double MINIMUM_DIRECTION = 0;

    // Save instance state keys
    public static final String STATE_CAMERA_POSITION = "cameraPosition";
    public static final String STATE_ZOOM_ENABLED = "zoomEnabled";
    public static final String STATE_SCROLL_ENABLED = "scrollEnabled";
    public static final String STATE_ROTATE_ENABLED = "rotateEnabled";
    public static final String STATE_TILT_ENABLED = "tiltEnabled";
    public static final String STATE_ZOOM_CONTROLS_ENABLED = "zoomControlsEnabled";
    public static final String STATE_DEBUG_ACTIVE = "debugActive";
    public static final String STATE_STYLE_URL = "styleUrl";
    public static final String STATE_ACCESS_TOKEN = "accessToken";
    public static final String STATE_DEFAULT_TRANSITION_DURATION = "defaultTransitionDuration";
    public static final String STATE_MY_LOCATION_ENABLED = "myLocationEnabled";
    public static final String STATE_MY_LOCATION_TRACKING_MODE = "myLocationTracking";
    public static final String STATE_MY_BEARING_TRACKING_MODE = "myBearingTracking";
    public static final String STATE_COMPASS_ENABLED = "compassEnabled";
    public static final String STATE_COMPASS_GRAVITY = "compassGravity";
    public static final String STATE_COMPASS_MARGIN_LEFT = "compassMarginLeft";
    public static final String STATE_COMPASS_MARGIN_TOP = "compassMarginTop";
    public static final String STATE_COMPASS_MARGIN_RIGHT = "compassMarginRight";
    public static final String STATE_COMPASS_MARGIN_BOTTOM = "compassMarginBottom";
    public static final String STATE_LOGO_GRAVITY = "logoGravity";
    public static final String STATE_LOGO_MARGIN_LEFT = "logoMarginLeft";
    public static final String STATE_LOGO_MARGIN_TOP = "logoMarginTop";
    public static final String STATE_LOGO_MARGIN_RIGHT = "logoMarginRight";
    public static final String STATE_LOGO_MARGIN_BOTTOM = "logoMarginBottom";
    public static final String STATE_LOGO_VISIBILITY = "logoVisibility";
    public static final String STATE_ATTRIBUTION_GRAVITY = "attrGravity";
    public static final String STATE_ATTRIBUTION_MARGIN_LEFT = "attrMarginLeft";
    public static final String STATE_ATTRIBUTION_MARGIN_TOP = "attrMarginTop";
    public static final String STATE_ATTRIBUTION_MARGIN_RIGHT = "attrMarginRight";
    public static final String STATE_ATTRIBUTION_MARGIN_BOTTOM = "atrrMarginBottom";
    public static final String STATE_ATTRIBUTION_VISIBILITY = "atrrVisibility";

    public static final String TAG = "MapboxMap";
}
