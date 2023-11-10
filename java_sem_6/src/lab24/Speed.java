/**
 * Напишите функцию, на входе которой расстояние до места назначения (S, м) и время в пути (T, мин).
 * Вычислить скорость (км/ч). Напишите тесты.
 */
package lab24;

public class Speed {
    public static double calculateSpeed(int distanceInMeters, int timeInMinutes) {
        double speedMM = 0;

        if (distanceInMeters < 0 || timeInMinutes < 0) {
            System.out.println("Wrong arguments! Out default result (0)");
            return 0;
        }

        try {
            speedMM = (double) (distanceInMeters / timeInMinutes);
        } catch (Exception e)
        {
            System.out.println("Program throw exception:" + e.toString());
            System.out.println("Out default result (0)");
        }

        double speedKH = speedMM * 0.06;

        return ((double) Math.round(speedKH * 100) / 100);
    }
}