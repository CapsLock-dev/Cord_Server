-- migrate:up
CREATE TABLE direct_messages (
    id BIGSERIAL PRIMARY KEY,
    sender_id BIGINT NOT NULL REFERENCES users (id) ON DELETE CASCADE,
    recipient_id BIGINT NOT NULL REFERENCES users (id) ON DELETE CASCADE,
    content TEXT NOT NULL,
    sent_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),

    CONSTRAINT dm_no_self_message CHECK (sender_id != recipient_id)
);

CREATE INDEX dm_conversation_idx ON direct_messages (
    LEAST(sender_id, recipient_id),
    GREATEST(sender_id, recipient_id),
    sent_at DESC,
    id DESC
);

-- migrate:down
DROP TABLE direct_messages;
