package lab24;

import org.junit.jupiter.api.Test;

import static lab24.Speed.calculateSpeed;
import static org.junit.jupiter.api.Assertions.*;

class SpeedTest {

    @Test
    void testCalculateSpeed() {
        assertEquals(calculateSpeed(0     , 0)    , 0);
        assertEquals(calculateSpeed(10000 , 0)    , 0);
        assertEquals(calculateSpeed(0     , 10000), 0);

        assertEquals(calculateSpeed(100, 10), 0.6);
        assertEquals(calculateSpeed(585, 13), 2.7);
        assertEquals(calculateSpeed(585, 13), 2.7);

        assertEquals(calculateSpeed(   1,    1), 0.06);
        assertEquals(calculateSpeed(1000, 1000), 0.06);

        assertEquals(calculateSpeed(  -1,  100), 0);
        assertEquals(calculateSpeed( 100,   -1), 0);
        assertEquals(calculateSpeed(-100, -100), 0);
    }
}