FROM gradle:9.3.1-jdk25 AS build
WORKDIR /app

COPY build.gradle ./
COPY src ./src

RUN gradle bootJar -x test

FROM eclipse-temurin:25-jre

WORKDIR /app

COPY --from=build /app/build/libs/*.jar app.jar

EXPOSE 8080
ENTRYPOINT ["java", "-jar", "app.jar"]
