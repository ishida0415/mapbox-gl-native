package com.mapbox.mapboxsdk.geometry;

import org.junit.Test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

public class ProjectedMetersTest {

    @Test
    public void testSanity() {
        ProjectedMeters projectedMeters = new ProjectedMeters(0.0, 0.0);
        assertNotNull("projectedMeters should not be null", projectedMeters);
    }

    @Test
    public void testNorthing() {
        ProjectedMeters projectedMeters = new ProjectedMeters(1.0, 0.0);
        assertEquals("northing should be 1", 1, projectedMeters.getNorthing(), 0);
    }

    @Test
    public void testEasting() {
        ProjectedMeters projectedMeters = new ProjectedMeters(0.0, 1.0);
        assertEquals("easting should be 1", 1, projectedMeters.getEasting(), 0);
    }

    @Test
    public void testConstructor() {
        ProjectedMeters projectedMeters1 = new ProjectedMeters(1, 2);
        ProjectedMeters projectedMeters2 = new ProjectedMeters(projectedMeters1);
        assertEquals("projectedmeters should match", projectedMeters1, projectedMeters2);
    }

    @Test
    public void testHashcode() {
        ProjectedMeters meters = new ProjectedMeters(1, 2);
        assertEquals("hashcode should match", -1048576, meters.hashCode());
    }

    @Test
    public void testToString(){
        ProjectedMeters meters = new ProjectedMeters(1, 1);
        assertEquals("toString should match","ProjectedMeters [northing=1.0, easting=1.0]",meters.toString());
    }
}
