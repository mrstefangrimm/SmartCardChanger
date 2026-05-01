using SmartCardStatus.Specs.Apps;

namespace SmartCardStatus.Specs.Services;

public interface ISmartCardStatusService
{
    SmartCardStatusAppMainWindow MainWindow { get; }
}

public class SmartCardStatusService(SmartCardStatusAppMainWindow mainWindow) : ISmartCardStatusService
{
    public SmartCardStatusAppMainWindow MainWindow { get; } = mainWindow;
}
