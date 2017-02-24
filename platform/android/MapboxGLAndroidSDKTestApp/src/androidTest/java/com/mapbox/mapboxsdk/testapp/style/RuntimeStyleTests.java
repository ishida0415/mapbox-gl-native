package com.mapbox.mapboxsdk.testapp.style;

import android.graphics.Color;
import android.graphics.PointF;
import android.support.test.espresso.Espresso;
import android.support.test.espresso.UiController;
import android.support.test.espresso.ViewAction;
import android.support.test.rule.ActivityTestRule;
import android.support.test.runner.AndroidJUnit4;
import android.view.View;

import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.style.layers.CannotAddLayerException;
import com.mapbox.mapboxsdk.style.layers.FillLayer;
import com.mapbox.mapboxsdk.style.layers.Layer;
import com.mapbox.mapboxsdk.style.layers.Property;
import com.mapbox.mapboxsdk.style.layers.PropertyFactory;
import com.mapbox.mapboxsdk.style.sources.CannotAddSourceException;
import com.mapbox.mapboxsdk.style.sources.Source;
import com.mapbox.mapboxsdk.style.sources.VectorSource;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.style.RuntimeStyleTestActivity;
import com.mapbox.mapboxsdk.testapp.utils.OnMapReadyIdlingResource;
import com.mapbox.mapboxsdk.testapp.utils.ViewUtils;

import junit.framework.Assert;

import org.hamcrest.Matcher;
import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.util.List;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.matcher.ViewMatchers.isDisplayed;
import static android.support.test.espresso.matcher.ViewMatchers.withId;
import static junit.framework.Assert.assertTrue;
import static junit.framework.Assert.fail;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;

/**
 * Basic smoke tests for Layer and Source
 */
@RunWith(AndroidJUnit4.class)
public class RuntimeStyleTests {

  @Rule
  public final ActivityTestRule<RuntimeStyleTestActivity> rule = new ActivityTestRule<>(RuntimeStyleTestActivity.class);

  private OnMapReadyIdlingResource idlingResource;

  @Before
  public void registerIdlingResource() {
    idlingResource = new OnMapReadyIdlingResource(rule.getActivity());
    Espresso.registerIdlingResources(idlingResource);
  }

  @Test
  public void testListLayers() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    onView(withId(R.id.mapView)).perform(new BaseViewAction() {

      @Override
      public void perform(UiController uiController, View view) {
        MapboxMap mapboxMap = rule.getActivity().getMapboxMap();

        List<Layer> layers = mapboxMap.getLayers();
        assertNotNull(layers);
        assertTrue(layers.size() > 0);
        for (Layer layer : layers) {
          assertNotNull(layer);
        }
      }

    });
  }

  @Test
  public void testGetAddRemoveLayer() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    onView(withId(R.id.mapView)).perform(new AddRemoveLayerAction());
  }

  @Test
  public void testAddRemoveSource() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);

    MapboxMap mapboxMap = rule.getActivity().getMapboxMap();
    mapboxMap.addSource(new VectorSource("my-source", "mapbox://mapbox.mapbox-terrain-v2"));
    mapboxMap.removeSource("my-source");

    onView(withId(R.id.mapView)).perform(new AddRemoveSourceAction());
  }

  /**
   * https://github.com/mapbox/mapbox-gl-native/issues/7973
   */
  @Test
  public void testQueryRenderedFeaturesInputHandling() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    onView(withId(R.id.mapView)).perform(new BaseViewAction() {

      @Override
      public void perform(UiController uiController, View view) {
        MapboxMap mapboxMap = rule.getActivity().getMapboxMap();
        String[] layerIds = new String[600];
        for (int i = 0; i < layerIds.length; i++) {
          layerIds[i] = "layer-" + i;
        }
        mapboxMap.queryRenderedFeatures(new PointF(100, 100), layerIds);
      }

    });
  }

  private class AddRemoveLayerAction extends BaseViewAction {

    @Override
    public void perform(UiController uiController, View view) {
      MapboxMap mapboxMap = rule.getActivity().getMapboxMap();

      // Get initial
      assertNotNull(mapboxMap.getLayer("building"));

      // Remove
      mapboxMap.removeLayer("building");
      assertNull(mapboxMap.getLayer("building"));

      // Add
      FillLayer layer = new FillLayer("building", "composite");
      layer.setSourceLayer("building");
      mapboxMap.addLayer(layer);
      assertNotNull(mapboxMap.getLayer("building"));

      // Assure the reference still works
      layer.setProperties(PropertyFactory.visibility(Property.VISIBLE));

      // Remove, preserving the reference
      mapboxMap.removeLayer(layer);

      // Property setters should still work
      layer.setProperties(PropertyFactory.fillColor(Color.RED));

      // Re-add the reference...
      mapboxMap.addLayer(layer);

      // Ensure it's there
      Assert.assertNotNull(mapboxMap.getLayer(layer.getId()));

      // Test adding a duplicate layer
      try {
        mapboxMap.addLayer(new FillLayer("building", "composite"));
        fail("Should not have been allowed to add a layer with a duplicate id");
      } catch (CannotAddLayerException cannotAddLayerException) {
        // OK
      }
    }
  }

  private class AddRemoveSourceAction extends BaseViewAction {

    @Override
    public void perform(UiController uiController, View view) {
      MapboxMap mapboxMap = rule.getActivity().getMapboxMap();

      // Add initial source
      mapboxMap.addSource(new VectorSource("my-source", "mapbox://mapbox.mapbox-terrain-v2"));

      // Remove
      mapboxMap.removeSource("my-source");
      assertNull(mapboxMap.getLayer("my-source"));

      // Add
      Source source = new VectorSource("my-source", "mapbox://mapbox.mapbox-terrain-v2");
      mapboxMap.addSource(source);

      // Remove, preserving the reference
      mapboxMap.removeSource(source);

      // Re-add the reference...
      mapboxMap.addSource(source);

      // Ensure it's there
      Assert.assertNotNull(mapboxMap.getSource(source.getId()));

      // Test adding a duplicate source
      try {
        Source source2 = new VectorSource("my-source", "mapbox://mapbox.mapbox-terrain-v2");
        mapboxMap.addSource(source2);
        fail("Should not have been allowed to add a source with a duplicate id");
      } catch (CannotAddSourceException cannotAddSourceException) {
        // OK
      }
    }
  }

  @After
  public void unregisterIntentServiceIdlingResource() {
    Espresso.unregisterIdlingResources(idlingResource);
  }

  public abstract class BaseViewAction implements ViewAction {

    @Override
    public Matcher<View> getConstraints() {
      return isDisplayed();
    }

    @Override
    public String getDescription() {
      return getClass().getSimpleName();
    }

  }
}
