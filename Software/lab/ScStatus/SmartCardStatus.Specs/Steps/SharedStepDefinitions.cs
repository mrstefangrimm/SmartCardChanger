using Reqnroll;

namespace SmartCardStatus.Specs.Steps;

[Binding]
internal sealed class SharedStepDefinitions
{
  [Given("wait for {int} seconds")]
  public void GivenWaitForSeconds(int p0)
  {
    Thread.Sleep(TimeSpan.FromSeconds(p0));
  }

  [When("wait for {int} seconds")]
  public void WhenWaitForSeconds(int p0)
  {
    Thread.Sleep(TimeSpan.FromSeconds(p0));
  }
}
