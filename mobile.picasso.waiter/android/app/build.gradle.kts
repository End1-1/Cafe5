import java.util.Properties
import java.io.FileInputStream
import com.android.build.api.artifact.Artifact

plugins {
    id("com.android.application")
    id("kotlin-android")
    id("dev.flutter.flutter-gradle-plugin")
}

val localProperties = Properties().apply {
    val localPropertiesFile = rootProject.file("local.properties")
    if (localPropertiesFile.exists()) {
        localPropertiesFile.reader(Charsets.UTF_8).use { reader ->
            load(reader)
        }
    }
}

val keystorePropertiesFile = rootProject.file("key.properties")
val keystoreProperties = Properties().apply {
    if (keystorePropertiesFile.exists()) {
        load(FileInputStream(keystorePropertiesFile))
    }
}

/** Paths in key.properties are relative to the android/ folder (not android/app/). */
fun resolveKeystoreFile(path: String): java.io.File {
    val fromAndroidRoot = rootProject.file(path)
    if (fromAndroidRoot.exists()) {
        return fromAndroidRoot
    }
    return file(path)
}

val releaseKeystoreFile = keystoreProperties["storeFile"]?.let { path ->
    resolveKeystoreFile(path as String)
}
val hasReleaseSigning = keystorePropertiesFile.exists() &&
    releaseKeystoreFile != null &&
    releaseKeystoreFile.exists()

android {
    namespace = "com.picassowaiter"
    compileSdk = 36
    ndkVersion = "27.0.12077973"

    defaultConfig {
        applicationId = "com.picassowaiter"
        minSdk = flutter.minSdkVersion
        targetSdk = flutter.targetSdkVersion
        versionCode = flutter.versionCode
        versionName = flutter.versionName

        setProperty("archivesBaseName", "picasso.waiter.$versionCode")
    }

    signingConfigs {
        if (hasReleaseSigning) {
            create("release") {
                keyAlias = keystoreProperties["keyAlias"] as String
                keyPassword = keystoreProperties["keyPassword"] as String
                storeFile = releaseKeystoreFile
                storePassword = keystoreProperties["storePassword"] as String
            }
        }
    }

    buildTypes {
        getByName("release") {
            signingConfig = if (hasReleaseSigning) {
                signingConfigs.getByName("release")
            } else {
                signingConfigs.getByName("debug")
            }
        }
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_11
        targetCompatibility = JavaVersion.VERSION_11
    }

    kotlinOptions {
        jvmTarget = JavaVersion.VERSION_11.toString()
    }

    sourceSets["main"].java.srcDirs("src/main/kotlin")
}

flutter {
    source = "../.."
}

dependencies {
    implementation("com.google.mlkit:barcode-scanning:17.3.0")
}


