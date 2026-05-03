using SmartCardStatus.Specs.Apps;

namespace SmartCardStatus.Specs.Services;

public class SmartCardChangerService(SmartCardChangerApiClient api)
{
    public SmartCardChangerApiClient App { get; } = api;
}
