# Subtitle Recognition

## Requirements

- C++17
- Boost: `program-options`
- \{fmt\}
- OpenCV 4
- Tesseract (with tessdata)

## Commandline Options

| Short Option | Long Option      | Default Value  | Description                                                                |
| ------------ | ---------------- | -------------- | -------------------------------------------------------------------------- |
|   `-h`       | `--help`         | /              | Print help message.                                                        |
|   `-g`       | `--search-gap`   | 0.16           | Gap seconds when searching in the video.                                   |
|   `-m`       | `--min-duration` | 0.5            | Minimum duration seconds of each subtitle, skip that long when detected.   |
|   `-t`       | `--threshold`    | 239            | Threshold value used when binarizing images (0~255).                       |
|   `-c`       | `--min-conf`     | 85             | Minimum confidence value of OCR results.                                   |
|   `-i`       | `--input-file`   | \<*Required*\> | Path to the video file to be processed.                                    |
|   `-b`       | `--box`          | \<*Required*\> | Detection box (x, y, w, h).                                                |
|   `-l`       | `--lang`         | \<*Required*\> | Language configuration string (same as Tesseract's `LANG`).                |
|   `-d`       | `--tessdata-dir` | \<*Required*\> | Path to tessdata directory.                                                |
