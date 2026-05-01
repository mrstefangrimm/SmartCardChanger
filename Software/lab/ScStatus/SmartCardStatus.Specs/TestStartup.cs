using Autofac;
using Reqnroll.Amp;
using Reqnroll.Autofac;
using SmartCardStatus.Specs.Apps;
using SmartCardStatus.Specs.Services;
using SmartCardStatus.Specs.Steps;
using Xunit;

[assembly: CollectionBehavior(DisableTestParallelization = true)]

namespace SmartCardStatus.Specs;

public static class TestStartup
{
  [ScenarioDependencies]
  public static void CreateServices(ContainerBuilder builder)
  {
    builder.RegisterConfiguration();
    builder.RegisterAppSettings();
    builder.RegisterScreenCapturing();

    builder.RegisterType<SharedStepDefinitions>().InstancePerDependency();

    builder.RegisterType<SmartCardChangerStepDefinitions>().InstancePerDependency();
    builder.RegisterType<SmartCardChangerService>().AsSelf().InstancePerLifetimeScope();
    builder.RegisterType<SmartCardChangerApiClient>().AsSelf().InstancePerDependency();
    builder.RegisterType<HttpClientDriver>().AsSelf().InstancePerLifetimeScope();

    builder.RegisterType<SmartCardStatusStepDefinitions>().InstancePerDependency();
    builder.RegisterType<SmartCardStatusAppMainWindow>().AsSelf().InstancePerDependency();
    builder.RegisterType<SmartCardStatusService>().As<ISmartCardStatusService>().InstancePerLifetimeScope();
    builder.RegisterType<FlaUIDriver>().AsSelf().InstancePerLifetimeScope();
  }
}
