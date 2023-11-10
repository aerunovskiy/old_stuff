/*
 * Написать аннотацию с целочисленным параметром. Создать класс, содержащий только приватные методы (3-4шт),
 * аннотировать любые из них. Вызвать из другого класса все аннотированные методы столько раз, сколько указано в
 * параметре аннотации.
 */

package lab8;

import java.lang.reflect.InvocationTargetException;

public class Annotator
{
    public static void main(String[] args)
    {
        try
        {
            SomePrivateClass spc = new SomePrivateClass();
            Class<?> annotated_class = spc.getClass(); //Метод getClass позволяет получить тип данного объекта

            for (var method : annotated_class.getDeclaredMethods()) // Проходимся по методам класса
            {
                if (method.isAnnotationPresent(Repeater.class)) // Проверяет, есть ли нужная аннотация
                {
                    method.setAccessible(true); // Делаем метод доступным

                    for (int i = 0; i < method.getAnnotation(Repeater.class).iteration(); ++i) //
                        method.invoke(spc); // Вызываем объект method iteration раз
                }
            }
        }
        catch (IllegalAccessException | InvocationTargetException error)
        {
            System.out.println("Program trow exception: " + error.toString());
        }
    }
}
