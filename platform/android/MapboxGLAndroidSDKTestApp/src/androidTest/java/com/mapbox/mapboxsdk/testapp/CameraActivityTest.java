package com.mapbox.mapboxsdk.testapp;

import android.app.Activity;
import android.support.test.rule.ActivityTestRule;
import android.support.test.runner.AndroidJUnit4;
import android.test.suitebuilder.annotation.LargeTest;

import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

/**
 * Tests on CameraActivity
 */
@RunWith(AndroidJUnit4.class)
@LargeTest
public class CameraActivityTest extends BaseTest {

    @Rule
    public ActivityTestRule<CameraActivity> mActivityRule = new ActivityTestRule<>(CameraActivity.class);

    private Activity mActivity = null;

    @Before
    public void setActivity() {
        mActivity = mActivityRule.getActivity();
    }

    @Test
    public void testSanity() {
        checkViewIsDisplayed(R.id.cameraMapView);
    }
}

