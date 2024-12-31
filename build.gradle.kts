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
            cmake
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
}
//gradlew h264bitstream:publish
//afterEvaluate {
//    publishing {
//        publications {
//            register("h264bitstream", MavenPublication::class) {
//                from(components["release"])
//                groupId = "com.homesoft.android"
//                artifactId = "h264bitstream"
//                version = "0.2.0"
//            }
//        }
//    }
//}
