plugins {
    id("com.android.library")
}

android {
    namespace = "org.spatialite"
    compileSdk = 33

    defaultConfig {
        minSdk = 21
        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
        ndk {
            abiFilters.add("arm64-v8a")
            abiFilters.add("armeabi-v7a")
            abiFilters.add("x86_64")
        }
    }

    buildTypes {
        release {
            isMinifyEnabled = false
            proguardFiles(getDefaultProguardFile("proguard-android-optimize.txt"), "proguard-rules.pro")
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_1_8
        targetCompatibility = JavaVersion.VERSION_1_8
    }
    ndkVersion = "23.1.7779620"
    sourceSets {
        getByName("main") {
            jniLibs.srcDir("src/main/jniLibs")
            jni.srcDirs("src/main/jni")
        }
    }
    externalNativeBuild {

        cmake {
            version = "3.22.1"
            path = file("src/main/jni/CMakeLists.txt")
        }


    }

}

tasks.register<Exec>("buildStaticLibs") {
    workingDir("${project.rootDir}/static_libs")
    commandLine("./masterbuild.sh")
    environment("NDK_HOME", project.extensions.getByType<com.android.build.gradle.BaseExtension>().ndkDirectory)
    environment("PROJECT_ROOT", project.rootDir)
}


tasks.named("preBuild") {
    dependsOn("buildStaticLibs")
}




dependencies {

testImplementation("junit:junit:4.13.2")
androidTestImplementation("androidx.test.ext:junit:1.1.5")
androidTestImplementation("androidx.test.espresso:espresso-core:3.5.1")
}