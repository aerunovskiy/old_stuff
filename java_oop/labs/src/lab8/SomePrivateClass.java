package lab8;

public class SomePrivateClass {
    @Repeater
    private static void sayWoof()
    {
        System.out.println("Woof!");
    }

    @Repeater(iteration = 2)
    public static void sayMeow()
    {
        System.out.println("Meow!");
    }

    @Repeater(iteration = 3)
    private static void sayOink()
    {
        System.out.println("Oink!");
    }

    @Repeater(iteration = 4)
    private static void sayMoo()
    {
        System.out.println("Moo!");
    }
}