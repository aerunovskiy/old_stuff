package lab8;

import java.lang.annotation.ElementType;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Retention;
import java.lang.annotation.Target;

/*
 * Аннотация @Retention позволяет указать, в какой момент жизни программного кода будет доступна аннотация:
 * только в исходном коде, в скомпилированном классе или во время выполнения программы
 */
@Retention(RetentionPolicy.RUNTIME)
//Аннотацией @Target указывается, какой элемент программы будет использоваться аннотацией
@Target(ElementType.METHOD)
public @interface Repeater
{
    int iteration() default 1;
}
