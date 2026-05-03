using System.Net.Http.Json;
using System.Text;
using System.Text.Json;
using Reqnroll.Amp;

namespace SmartCardStatus.Specs.Apps;

public class SmartCardChangerApiClient(HttpClientDriver driver)
{
  public async Task<string> SendCommand(string comPort, string command)
  {
    string json = JsonSerializer.Serialize(new SmartCardChangerRequest(command));
    var content = new StringContent(json, Encoding.UTF8, "application/json");

    var response = await driver.Stub.PatchAsync(comPort, content);
    response.EnsureSuccessStatusCode();

    var result = await response.Content.ReadFromJsonAsync<SmartCardChangerResponse>();
    return result.Output;
  }
}

public record SmartCardChangerRequest(string command)
{
}

public record SmartCardChangerResponse(bool success, string Output, string Error)
{
}
