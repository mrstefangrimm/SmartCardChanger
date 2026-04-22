using PCSC;

namespace SmartCardStatus;

public static class SmartCardStatus
{

    public static bool IsCardInserted()
    {
        try
        {
            Console.WriteLine("Eis");
            var contextFactory = ContextFactory.Instance;
            Console.WriteLine("Zwei");
            using (var ctx = contextFactory.Establish(SCardScope.System))
            {
                Console.WriteLine("Currently connected readers: ");
                var readerNames = ctx.GetReaders();
                foreach (var readerName in readerNames)
                {
                    if (readerName == "HID Global OMNIKEY 3x21 Smart Card Reader 0")
                    {
                        var status = ctx.GetReaderStatus(readerName);
                        Console.WriteLine("\t" + readerName + status.CurrentState);

                        if ((status.EventState & SCRState.Empty) == SCRState.Empty) return false;
                        if ((status.EventState & SCRState.Mute) == SCRState.Mute) return false;

                        using (var reader = ctx.ConnectReader(readerName, SCardShareMode.Shared, SCardProtocol.Any))
                        {
                            var cardAtr = reader.GetAttrib(SCardAttribute.AtrString);
                            return true;
                        }
                    }
                }
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine("ExMsg" + ex.Message);
            return false;
        }
        return false;
    }

}
