# See https://aka.ms/customizecontainer to learn how to customize your debug container and how Visual Studio uses this Dockerfile to build your images for faster debugging.

# This stage is used when running from VS in fast mode (Default for Debug configuration)
FROM mcr.microsoft.com/dotnet/aspnet:10.0 AS base

RUN apt-get update && apt-get install -y --no-install-recommends \
    libpcsclite1 pcscd pcsc-tools && rm -rf /var/lib/apt/lists/*
    
USER $APP_UID
WORKDIR /app
EXPOSE 8080

# This stage is used to build the service project
FROM mcr.microsoft.com/dotnet/sdk:10.0 AS build
#FROM mcr.microsoft.com/dotnet/sdk:10.0-bullseye AS build
ARG BUILD_CONFIGURATION=Release
WORKDIR /src
COPY . .
# COPY ./SmartCardStatusApi/SmartCardStatusApi.csproj ./SmartCardStatusApi/
RUN dotnet restore ./SmartCardStatusApi/SmartCardStatusApi.csproj
#COPY ./SmartCardStatusApi ./SmartCardStatusApi
#COPY ./SmartCardStatus ./SmartCardStatus
RUN dotnet build ./SmartCardStatusApi/SmartCardStatusApi.csproj -c $BUILD_CONFIGURATION -o /app/build

#RUN apt-get update && apt-get install -y --no-install-recommends \
#    libpcsclite1 pcscd pcsc-tools && rm -rf /var/lib/apt/lists/*


# This stage is used to publish the service project to be copied to the final stage
FROM build AS publish
ARG BUILD_CONFIGURATION=Release
RUN dotnet publish ./SmartCardStatusApi/SmartCardStatus.csproj -c $BUILD_CONFIGURATION -o /app/publish /p:UseAppHost=false

#RUN apt-get update && apt-get install -y libpcsclite1 pcscd pcsc-tools

# This stage is used in production or when running from VS in regular mode (Default when not using the Debug configuration)
FROM base AS final
WORKDIR /app
COPY --from=publish /app/publish .
#ENTRYPOINT ["dotnet", "SmartCardStatusApi.dll"]

COPY entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]
