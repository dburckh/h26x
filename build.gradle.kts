// Top-level build file where you can add configuration options common to all sub-projects/modules.
plugins {
    alias(libs.plugins.android.library)
    alias(libs.plugins.maven.publish)
}

android {
    namespace = "com.homesoft.h26x"
    compileSdk = 34

    defaultConfig {
        minSdk = 21
        externalNativeBuild {
            cmake {
                arguments.add("-DANDROID_STL=c++_shared")
            }
        }
    }
    buildFeatures {
        prefab = true
        prefabPublishing = true
    }

    prefab {
        create("h26x") {
            headers = "h26x/include"
        }
    }
    externalNativeBuild {
        cmake {
            path("CMakeLists.txt")
        }
    }
    publishing {
        singleVariant("release")
    }
    packaging.jniLibs.excludes.add("**.so")
}
afterEvaluate {
    publishing {
        publications {
            register("h26x", MavenPublication::class) {
                from(components["release"])
                groupId = "com.homesoft.android"
                artifactId = "h26x"
                version = "0.1.2"
            }
        }
    }
}
