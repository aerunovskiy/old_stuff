package lab17;
public class SpeakerExecutor {
    public static void main(String[] args) {
        HelloSpeaker helloSpeaker = new HelloSpeaker();
        RussianSpeaker russianSpeaker = new RussianSpeaker();

        helloSpeaker.sayHello();
        russianSpeaker.sayHello();
    }
}
