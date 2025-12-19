# img-store

Fast C++20 image storage server with content-addressed storage and named aliases.

## Build

```bash
make
```

## Run

```bash
./build/bin/img-store --port 8080 --storage ./storage
```

## Docker

```bash
docker compose up -d
```

Set `IMG_STORE_API_KEY` in `.env` for authentication.

## Usage

```bash
# Upload with name
curl -X POST http://localhost:8080/image.png \
  -H "Authorization: Bearer your-key" \
  --data-binary @file.png

# Download
curl http://localhost:8080/image.png

# Upload by hash
curl -X POST http://localhost:8080/images \
  -H "Authorization: Bearer your-key" \
  --data-binary @file.png

# Download by hash
curl http://localhost:8080/images/<hash>
```

## License

MIT
